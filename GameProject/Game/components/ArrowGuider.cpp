#include "ArrowGuider.h"

#include <Engine/Entity/Entity.h>

ArrowGuider::ArrowGuider(Entity* parentEntity, float movementSpeed, float maxTurnSpeed)
    :Component(parentEntity, "ArrowGuider")
{
    isGuiding = false;

    this->movementSpeed = movementSpeed;

    this->maxTurnSpeed = maxTurnSpeed;

    // Window resolution (in one axis) is used to separate mouse movement
    // from the window resolution
    EventBus::get().subscribe(this, &ArrowGuider::handleWindowResize);
    windowHeight = Display::get().getHeight();

    turnFactors = glm::vec2(0.0f, 0.0f);

    posStoreTimer = 0.0f;

    arrowCamera = nullptr;
}

ArrowGuider::~ArrowGuider()
{
    EventBus::get().unsubscribe(this, &ArrowGuider::handleWindowResize);
    EventBus::get().unsubscribe(this, &ArrowGuider::handleMouseMove);
}

void ArrowGuider::update(const float& dt)
{
    if (!isGuiding) {
        // Do nothing if the guider is disabled
        return;
    }

    glm::vec3 direction = host->getTransform()->getForward();

    // Update arrow position
    glm::vec3 currentPos = getPosition();
    glm::vec3 newPos = currentPos + direction * movementSpeed * dt;

    setPosition(newPos);

    // Update turn factors (proportionally slow down turning)
    turnFactors.x /= 1.0f + turnFactorFalloff * dt;
    turnFactors.y /= 1.0f + turnFactorFalloff * dt;

    applyTurn();

    // Update position storing frequency based on turning factors
    float turnFactorsLength = glm::length(turnFactors);
    posStoreFrequency = minStoreFrequency + (maxStoreFrequency - minStoreFrequency) * turnFactorsLength;

    // Update position store timer
    posStoreTimer += dt;

    if (posStoreTimer > 1.0f/posStoreFrequency) {
        // Store position and reset timer
        std::fmod(posStoreTimer, posStoreFrequency);

        storedPositions.push_back(getPosition());
    }

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

void ArrowGuider::startGuiding()
{
    // Get camera pointer from parent entity
    Component* tempPtr = host->getComponent("Camera");
    arrowCamera = dynamic_cast<Camera*>(tempPtr);

    if (!arrowCamera) {
        LOG_WARNING("Arrow Guider failed to find Camera component, will not start guiding");
        return;
    }

    isGuiding = true;

    EventBus::get().subscribe(this, &ArrowGuider::handleMouseMove);

    // Clear previous path and store starting position
    storedPositions.clear();
    storedPositions.push_back(getPosition());


    // Set camera settings
    arrowCamera->setFOV(minFOV);
    arrowCamera->setOffset(minCamOffset);
}

void ArrowGuider::stopGuiding()
{
    isGuiding = false;

    EventBus::get().unsubscribe(this, &ArrowGuider::handleMouseMove);

    turnFactors.x = 0.0f;
    turnFactors.y = 0.0f;

    posStoreTimer = 0.0f;
}

void ArrowGuider::handleMouseMove(MouseMoveEvent* event)
{
    // Divide by window height to separate turns from the screen resolution
    // Divide by max mouse move to get the turn factor [-1,1]
    float turnFactorYaw = (float)event->moveX / (windowHeight * maxMouseMove);
    // Invert Y as input treats Y as pointing downwards
    float turnFactorPitch = (float)-event->moveY / (windowHeight * maxMouseMove);

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

glm::vec3 ArrowGuider::getDirection()
{
    return host->getTransform()->getForward();
}

void ArrowGuider::setDirection(const glm::vec3 direction)
{
    host->getTransform()->setForward(direction);
}

glm::vec3 ArrowGuider::getPosition()
{
    return host->getTransform()->getPosition();
}

void ArrowGuider::setPosition(const glm::vec3 position)
{
    host->getTransform()->setPosition(position);
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

std::vector<glm::vec3>& ArrowGuider::getStoredPositions()
{
    return storedPositions;
}

float ArrowGuider::getTurningSpeed()
{
    return glm::length(turnFactors) * maxTurnSpeed;
}

void ArrowGuider::applyTurn()
{
    // Rotations measured in radians, kept within [-maxTurnSpeed, maxTurnSpeed]
    float yaw = turnFactors.x * maxTurnSpeed;
    float pitch = turnFactors.y * maxTurnSpeed;

    glm::vec3 direction = host->getTransform()->getForward();

    // Redirect arrow
    glm::vec3 upDir(0.0f, 1.0f, 0.0f);
    glm::vec3 rightVec = glm::cross(direction, upDir);

    // vec4 is required for multiplication with matrices
    glm::vec4 tempDirection(direction.x, direction.y, direction.z, 1.0f);

    // Yaw
    glm::mat4 rotMatrix = glm::rotate(turnFactors.x, upDir);
    tempDirection = tempDirection * rotMatrix;

    // Pitch
    rotMatrix = glm::rotate(turnFactors.y, rightVec);
    tempDirection = tempDirection * rotMatrix;

    // Used for calculation of entity rotation
    glm::vec3 normOldDirection = glm::normalize(direction);

    direction = glm::vec3(tempDirection);

    // Rotate the entity
    glm::vec3 normNewDirection = glm::normalize(direction);
    glm::vec3 rotationAxis = glm::cross(normOldDirection, normNewDirection);

    float cosAngle = glm::dot(normOldDirection, normNewDirection);
    float rotationAngle = std::acosf(cosAngle);

    host->getTransform()->rotateAxis(rotationAngle, rotationAxis);

    host->getTransform()->setForward(direction);
}
