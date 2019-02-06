#include "ArrowGuider.h"

#include <Engine/Entity/Entity.h>

ArrowGuider::ArrowGuider(Entity* parentEntity, EntityMatrix startingTransform, glm::vec3 startingDirection,
    float movementSpeed, float maxTurnSpeed)
    :Component(parentEntity, "ArrowGuider")
{
    *(host->getMatrix()) = startingTransform;

    isGuiding = false;

    this->direction = startingDirection;

    this->movementSpeed = movementSpeed;

    this->maxTurnSpeed = maxTurnSpeed;

    // Window resolution (in one axis) is used to separate mouse movement
    // from the window resolution
    EventBus::get().subscribe(this, &ArrowGuider::handleWindowResize);
    windowHeight = Display::get().getHeight();

    turnFactors = glm::vec2(0.0f, 0.0f);

    posStoreTimer = 0.0f;
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

    // Update arrow position
    glm::vec3 currentPos = getPosition();
    glm::vec3 newPos = currentPos + direction * movementSpeed * dt;

    setPosition(newPos);

    // Update turn factors (proportionally slow down turning)
    turnFactors.x /= 1.0f + turnFactorFalloff * dt;
    turnFactors.y /= 1.0f + turnFactorFalloff * dt;

    applyTurn();

    // Update position storing frequency based on turning factors
    posStoreFrequency = minStoreFrequency + (maxStoreFrequency - minStoreFrequency) * glm::length(turnFactors);

    // Update position store timer
    posStoreTimer += dt;

    if (posStoreTimer > 1.0f/posStoreFrequency) {
        // Store position and reset timer
        std::fmod(posStoreTimer, posStoreFrequency);

        storedPositions.push_back(getPosition());
    }
}

void ArrowGuider::startGuiding()
{
    isGuiding = true;

    EventBus::get().subscribe(this, &ArrowGuider::handleMouseMove);

    // Clear previous path and store starting position
    storedPositions.clear();
    storedPositions.push_back(getPosition());
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
    return direction;
}

void ArrowGuider::setDirection(const glm::vec3 direction)
{
    this->direction = direction;
}

glm::vec3 ArrowGuider::getPosition()
{
    return host->getMatrix()->getPosition();
}

void ArrowGuider::setPosition(const glm::vec3 position)
{
    host->getMatrix()->setPosition(position);
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
    //glm::vec3 normOldDirection = glm::normalize(direction);

    direction = glm::vec3(tempDirection);

    // TODO: Rotate the entity (rotation around axis needs to be implemented in EntityMatrix first)
    /*
    glm::vec3 normNewDirection = glm::normalize(direction);
    glm::vec3 rotationAxis = glm::cross(normOldDirection, normNewDirection);

    float rotationAngle = glm::dot(normOldDirection, normNewDirection);
    */
}
