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

            allowKeypointOverride = true;
        }

        // Update arrow position
        glm::vec3 currentPos = transform->getPosition();
        glm::vec3 newPos = currentPos + transform->getForward() * movementSpeed * dt;

        transform->setPosition(newPos);
    }

	if (arrowCamera) {
        this->updateCamera(dt, turnFactorsLength);
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
    startingKeyPoint.t = 0.0f;

    path.push_back(startingKeyPoint);    
}

void ArrowGuider::stopGuiding(float flightTime)
{
    isGuiding = false;

    stopAiming();

    this->flightTime = flightTime;

    // Store end point
    KeyPoint newKeyPoint;
    newKeyPoint.Position = host->getTransform()->getPosition();
    newKeyPoint.t = this->flightTime;

    path.back() = newKeyPoint;
}

void ArrowGuider::saveKeyPoint(float flightTime)
{
    // Do not save the key point if one was just saved during the same frame update
    if (posStoreTimer < FLT_EPSILON * 10.0f || !allowKeypointOverride) {
        return;
    }

    posStoreTimer = 0.0f;
    path.back() = KeyPoint(host->getTransform()->getPosition(), flightTime);

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

    /*
        Drift camera logic: the camera is slowly dragged directly behind the arrow (with an offset)
        The camera is always facing the arrow
    */
    Transform* transform = host->getTransform();

    glm::vec3 arrowPos = transform->getPosition();
    glm::vec3 arrowForward = transform->getForward();

    glm::vec3 camPos = arrowCamera->getPosition();

    // Desired position is arrow position + offset
    glm::vec3 desiredPosition = arrowPos + (transform->getRight() * newOffset.x + transform->getUp() * newOffset.y + arrowForward * newOffset.z);

    // Point the camera wants to look at and is rotated around when drifting
    float offsetDistance = glm::length(arrowPos - desiredPosition);
    glm::vec3 lookAt = desiredPosition + arrowForward * offsetDistance;

    glm::vec3 camForward = glm::normalize(lookAt - camPos);

    // Angle between forward vectors
    float forwardAngle = std::acosf(glm::dot(camForward, arrowForward));

    if (forwardAngle > 0.001f) {
        // Calculate the angle to rotate the forward by
        float rotationAngle;

        if (forwardAngle > maxForwardAngle) {
            // The angle differential is too large, snap the forward to the maximum allowed forward differential
            rotationAngle = glm::max(angleCorrectionFactor * dt * forwardAngle, forwardAngle - maxForwardAngle);
        } else {
            // Smoothly rotate the forward
            rotationAngle = glm::min(angleCorrectionFactor * dt * forwardAngle, forwardAngle);
        }

        // Limit rotation angle in case of a large dt
        rotationAngle = glm::min(rotationAngle, forwardAngle);

        // Rotate camera forward
        glm::vec3 rotationAxis = glm::normalize(glm::cross(camForward, arrowForward));

        glm::quat rotation = {1.0f, 0.0f, 0.0f, 0.0f};

        rotation = glm::rotate(rotation, rotationAngle, rotationAxis);

        camForward = rotation * camForward;
    }

    // Reposition camera
    camPos = lookAt - camForward * offsetDistance;

    arrowCamera->setForward(camForward);
    arrowCamera->setPosition(camPos);
}
