#include "ArrowGuider.h"

#include <Engine/Entity/Entity.h>
#include <Game/Components/ArrowConfig.h>
#include <Utils/Logger.h>

ArrowGuider::ArrowGuider(Entity* parentEntity)
    :Component(parentEntity, "ArrowGuider"),
    isGuiding(false),
    isAiming(false),
    minCamOffset(Arrow::minCamOffset),
    maxCamOffset(Arrow::maxCamOffset),
    movementSpeed(Arrow::minSpeed),
    minSpeed(Arrow::minSpeed),
    maxSpeed(Arrow::maxSpeed),
    minFOV(Arrow::minFOV),
    maxFOV(Arrow::maxFOV),
    turnSpeed(Arrow::minTurnSpeed),
    minTurnSpeed(Arrow::minTurnSpeed),
    maxTurnSpeed(Arrow::maxTurnSpeed),
    turnFactors(glm::vec2(0.0f,0.0f)),
    posStoreTimer(0.0f),
    arrowCamera(nullptr),
    flightTime(0.0f)
{
    // Window resolution (in one axis) is used to separate mouse movement
    // from the window resolution
    EventBus::get().subscribe(this, &ArrowGuider::handleWindowResize);

    windowHeight = Display::get().getHeight();
    currentPitch = 0.0f;

	// Set up for acceleration variables
	this->acceleration = 3.0f;
	this->turnSpeedDeceleration = 3.0f * 0.2f;
	this->maxSpeedOffset = 1.0f;
	this->isAccelerating = false;

	sound.loadSound("./Game/assets/sound/Wind.wav");
	sound.setVolume(0.5);
	sound.setLoopState(true);
	SoundManager::get().addSound(&sound, SOUND_EFFECT);
}

ArrowGuider::~ArrowGuider()
{
    EventBus::get().unsubscribe(this, &ArrowGuider::handleWindowResize);

    EventBus::get().unsubscribe(this, &ArrowGuider::handleMouseMove);
    EventBus::get().unsubscribe(this, &ArrowGuider::handleKeyEvent);
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
		sound.setVolume(movementSpeed / 15);

        // Update position storage
        float desiredFrequency = minStoreFrequency + (maxStoreFrequency - minStoreFrequency) * (turnFactorsLength * 3.0f) * this->movementSpeed;

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
			newKeyPoint.UpVector = host->getTransform()->getUp();

            path.push_back(newKeyPoint);

            allowKeypointOverride = true;
        }

		if (this->isAccelerating && this->movementSpeed < this->maxSpeed) {
            this->movementSpeed = glm::min(maxSpeed, movementSpeed + acceleration * dt);
            this->turnSpeed = glm::min(maxTurnSpeed, turnSpeed + this->turnSpeedDeceleration * dt);
		}

		else if (!this->isAccelerating && this->movementSpeed > this->minSpeed) {
            this->movementSpeed = glm::max(minSpeed, movementSpeed - acceleration * dt);
            this->turnSpeed = glm::max(minTurnSpeed, turnSpeed - this->turnSpeedDeceleration * dt);
		}

        // Update arrow position
        glm::vec3 currentPos = transform->getPosition();
        glm::vec3 newPos = currentPos + transform->getForward() * movementSpeed * dt;

        transform->setPosition(newPos);
	}

	if (arrowCamera) {
		float speedOffsetFactor = (this->movementSpeed - this->minSpeed) / (this->maxSpeed - this->minSpeed);

		this->updateCamera(dt, turnFactorsLength + speedOffsetFactor);
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

        // Decouple the camera to allow for drifting
        arrowCamera->decouple();
	}

    // Subscribe to mouse movement for guiding
    EventBus::get().subscribe(this, &ArrowGuider::handleMouseMove);
}

void ArrowGuider::stopAiming()
{
    isAiming = false;

    EventBus::get().unsubscribe(this, &ArrowGuider::handleMouseMove);

    turnFactors.x = 0.0f;
    turnFactors.y = 0.0f;
}

void ArrowGuider::startGuiding()
{
    if (!isAiming) {
        startAiming();
    }

    isGuiding = true;
    flightTime = 0.0f;

    // Do not allow the starting point to be overwritten
    allowKeypointOverride = false;

    // Clear previous path and store starting position
    path.clear();

    KeyPoint startingKeyPoint;
    startingKeyPoint.Position = host->getTransform()->getPosition();
	startingKeyPoint.UpVector = host->getTransform()->getUp();
    startingKeyPoint.t = 0.0f;

    path.push_back(startingKeyPoint);    

	// Subscribe to key events
	EventBus::get().subscribe(this, &ArrowGuider::handleKeyEvent);

	sound.playSound();
}

void ArrowGuider::stopGuiding(float flightTime)
{
    isGuiding = false;

    stopAiming();

    this->flightTime = flightTime;

    // Store end point
    KeyPoint newKeyPoint;
    newKeyPoint.Position = host->getTransform()->getPosition();
	newKeyPoint.UpVector = host->getTransform()->getUp();
    newKeyPoint.t = this->flightTime;

    path.back() = newKeyPoint;

	sound.stopSound();
}

void ArrowGuider::saveKeyPoint(float flightTime)
{
    // Do not save the key point if one was just saved during the same frame update
    if (posStoreTimer < FLT_EPSILON * 10.0f) {
        return;
    }

    posStoreTimer = 0.0f;
	if (this->allowKeypointOverride)
		path.back() = KeyPoint(host->getTransform()->getPosition(), host->getTransform()->getUp(), flightTime);
	else
		path.push_back(KeyPoint(host->getTransform()->getPosition(), host->getTransform()->getUp(), flightTime));

	allowKeypointOverride = false;

}

void ArrowGuider::handleMouseMove(MouseMoveEvent* event)
{
    // Calculate relative mouse position
	int moveX = event->deltaX;
	int moveY = event->deltaY;

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

void ArrowGuider::handleKeyEvent(KeyEvent * event)
{
	if (event->action == GLFW_PRESS && event->key == GLFW_KEY_LEFT_SHIFT) 
		this->isAccelerating = true;
	else if (event->action == GLFW_RELEASE && event->key == GLFW_KEY_LEFT_SHIFT) 
		this->isAccelerating = false;
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
    //glm::vec2 yawPitch;
    float yaw = -turnFactors.x * turnSpeed * dt;
    float pitch = -turnFactors.y * turnSpeed * dt;

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

void ArrowGuider::updateCamera(const float& dt, const float& turnFactorsLength)
{
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

    glm::vec3 newOffset = currentOffset + deltaOffset;

    arrowCamera->setOffset(newOffset);
}
