#include "ArrowGuider.h"

#include <Engine/Entity/Entity.h>
#include <Utils/Logger.h>

ArrowGuider::ArrowGuider(Entity* parentEntity, glm::vec3 minCamOffset, float minFOV, float movementSpeed, float maxTurnSpeed)
    :Component(parentEntity, "ArrowGuider"),
    minCamOffset(minCamOffset),
    isGuiding(false),
    isAiming(false),
    movementSpeed(movementSpeed),
    maxTurnSpeed(maxTurnSpeed),
    turnFactors(glm::vec2(0.0f,0.0f)),
    posStoreTimer(0.0f),
    arrowCamera(nullptr),
    flightTime(0.0f),
    minFOV(minFOV)
{
    // Window resolution (in one axis) is used to separate mouse movement
    // from the window resolution
    EventBus::get().subscribe(this, &ArrowGuider::handleWindowResize);
    windowHeight = Display::get().getHeight();

    currentPitch = 0.0f;
}

ArrowGuider::~ArrowGuider()
{
    EventBus::get().unsubscribe(this, &ArrowGuider::handleWindowResize);
    if (isGuiding) {
        EventBus::get().unsubscribe(this, &ArrowGuider::handleMouseMove);
    }
}

void ArrowGuider::update(const float& dt)
{
    if (!isGuiding && !isAiming) {
        // Do nothing if the guider is disabled
        return;
    }

    Transform* transform = host->getTransform();

    applyTurn(dt);

    float turnFactorsLength = glm::length(turnFactors);

    if (isGuiding) {
        // Update position storage
        float desiredFrequency = minStoreFrequency + (maxStoreFrequency - minStoreFrequency) * turnFactorsLength;

        // Gradually increase storing frequency
        float deltaFrequency = (desiredFrequency - posStoreFrequency) * dt;

        if (std::abs(deltaFrequency) > maxStoreFrequencyDelta) {
            deltaFrequency *= deltaFrequency / maxStoreFrequencyDelta;
        }

        posStoreFrequency += deltaFrequency;

        // Update position store timer
        posStoreTimer += dt;

        flightTime += dt;

        if (posStoreTimer > 1.0f/posStoreFrequency) {
            // Store position and reset timer
            posStoreTimer = 0.0f;

            KeyPoint newKeyPoint;
            newKeyPoint.Position = transform->getPosition();
            newKeyPoint.t = flightTime;

            path.push_back(newKeyPoint);
        }

        // Update arrow position
        glm::vec3 currentPos = transform->getPosition();
        glm::vec3 newPos = currentPos + transform->getForward() * movementSpeed * dt;

        transform->setPosition(newPos);
    }

	if (arrowCamera) {
		// Update camera settings using turn factors
		// Camera FOV
		float currentFOV = arrowCamera->getFOV();

		// Linearly interpolate between min and max FOV
		float desiredFOV = minFOV + (maxFOV - minFOV) * turnFactorsLength;

		// Gradually increase FOV
		float deltaFOV = (desiredFOV - currentFOV) * dt;

		// Limit FOV change per second
		if (std::abs(deltaFOV) > FOVChangeMax) {
			deltaFOV *= deltaFOV / FOVChangeMax;
		}

		arrowCamera->setFOV(currentFOV + deltaFOV);

		// Camera offset
		glm::vec3 currentOffset = arrowCamera->getOffset();

		// Linearly interpolate between min and max offset
		glm::vec3 desiredOffset = minCamOffset + (maxCamOffset - minCamOffset) * turnFactorsLength;

		// Gradually increase offset
		glm::vec3 deltaOffset = (desiredOffset - currentOffset) * dt;

		// Limit offset change per second
		if (glm::length(deltaOffset) > offsetChangeMax) {
			deltaOffset *= deltaOffset / offsetChangeMax;
		}

		arrowCamera->setOffset(currentOffset + deltaOffset);
	}
}

void ArrowGuider::startAiming()
{
    isAiming = true;

    // Get camera pointer from parent entity
    Component* tempPtr = host->getComponent("Camera");
    arrowCamera = dynamic_cast<Camera*>(tempPtr);

    if (!arrowCamera) {
        LOG_WARNING("Arrow Guider failed to find Camera component, will still guide arrow with mouse");
    } else {
		// Set camera settings
		arrowCamera->setFOV(minFOV);
		arrowCamera->setOffset(minCamOffset);
	}

    // Lock cursor
    glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Subscribe to mouse movement for guiding
    EventBus::get().subscribe(this, &ArrowGuider::handleMouseMove);
}

void ArrowGuider::stopAiming()
{
    isAiming = false;

    EventBus::get().unsubscribe(this, &ArrowGuider::handleMouseMove);

    turnFactors.x = 0.0f;
    turnFactors.y = 0.0f;

    // Unlock cursor
    glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void ArrowGuider::startGuiding()
{
    if (!isAiming) {
        startAiming();
    }

    isGuiding = true;
    flightTime = 0.0f;

    // Clear previous path and store starting position
    path.clear();

    KeyPoint startingKeyPoint;
    startingKeyPoint.Position = host->getTransform()->getPosition();
    startingKeyPoint.t = 0.0f;

    path.push_back(startingKeyPoint);    
}

void ArrowGuider::stopGuiding()
{
    isGuiding = false;

    stopAiming();

    // Store end point
    KeyPoint newKeyPoint;
    newKeyPoint.Position = host->getTransform()->getPosition();
    newKeyPoint.t = flightTime;

    path.push_back(newKeyPoint);
}

void ArrowGuider::handleMouseMove(MouseMoveEvent* event)
{
    // Calculate relative mouse position
	int moveX = event->travelX;
	int moveY = event->travelY;

    // Divide by window height to separate turn speed from screen resolution
    float turnFactorYaw = moveX / (windowHeight * maxMouseMove);
    float turnFactorPitch = moveY / (windowHeight * maxMouseMove);

    // Update turnFactors
    turnFactors.x += turnFactorYaw;
    turnFactors.y += turnFactorPitch;

    // Keep the length of turnfactors within [0,1]
    float turnFactorLength = glm::length(turnFactors);

    if (turnFactorLength > 1.0f) {
        turnFactors /= turnFactorLength;
    }
}

void ArrowGuider::handleWindowResize(WindowResizeEvent* event)
{
    this->windowHeight = event->height;
}

float ArrowGuider::getMaxTurnSpeed()
{
    return maxTurnSpeed;
}

void ArrowGuider::setMaxTurnSpeed(const float maxTurnSpeed)
{
    this->maxTurnSpeed = maxTurnSpeed;
}

float ArrowGuider::getMovementSpeed()
{
    return movementSpeed;
}

void ArrowGuider::setMovementSpeed(const float movementSpeed)
{
    this->movementSpeed = movementSpeed;
}

float ArrowGuider::getPosStoreFrequency()
{
    return posStoreFrequency;
}

std::vector<KeyPoint>& ArrowGuider::getPath()
{
    return path;
}

float ArrowGuider::getTurningSpeed()
{
    return glm::length(turnFactors) * maxTurnSpeed;
}

void ArrowGuider::applyTurn(const float& dt)
{
    // Update turn factors (proportionally slow down turning)
    turnFactors.x /= 1.0f + turnFactorFalloff * dt;
    turnFactors.y /= 1.0f + turnFactorFalloff * dt;

    // Rotations measured in radians, kept within [-maxTurnSpeed, maxTurnSpeed]
    float yaw = -turnFactors.x * maxTurnSpeed * dt;
    float pitch = -turnFactors.y * maxTurnSpeed * dt;

    // Limit pitch
    float newPitch = currentPitch + pitch;

    if (newPitch > maxPitch) {
        pitch = maxPitch - currentPitch;
    } else if (newPitch < -maxPitch) {
        pitch = -maxPitch - currentPitch;
    }

    currentPitch += pitch;

    host->getTransform()->rotate(yaw, pitch);
}
