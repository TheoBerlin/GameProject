#include "ArrowGuider.h"

#include <Engine/Entity/Entity.h>

ArrowGuider::ArrowGuider(Entity* parentEntity, Transform startingTransform, glm::vec3 startingDirection,
    float movementSpeed, float maxTurnSpeed)
    :Component(parentEntity, "ArrowGuider")
{
    *(host->getTransform()) = startingTransform;

    isGuiding = false;

    this->direction = startingDirection;

    this->movementSpeed = movementSpeed;

    this->maxTurnSpeed = maxTurnSpeed;

    // Window resolution (in one axis) is used to separate mouse movement
    // from the window resolution
    EventBus::get().subscribe(this, &ArrowGuider::handleWindowResize);
    windowHeight = Display::get().getHeight();

    turnFactors = glm::vec2(0.0f, 0.0f);
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
    turnFactors.x /= 1.0f + turnFactorFalloff;
    turnFactors.y /= 1.0f + turnFactorFalloff;

    applyTurn();
}

void ArrowGuider::startGuiding()
{
    isGuiding = true;

    EventBus::get().subscribe(this, &ArrowGuider::handleMouseMove);
}

void ArrowGuider::stopGuiding()
{
    isGuiding = false;

    EventBus::get().unsubscribe(this, &ArrowGuider::handleMouseMove);
}

void ArrowGuider::handleMouseMove(MouseMoveEvent* event)
{
    // Divide by window height to separate turns from the screen resolution
    // Divide by max mouse move to get the turn factor [-1,1]
    float turnFactorYaw = (float)event->moveX / (windowHeight * maxMouseMove);
    // Invert Y as input treats Y as pointing downwards
    float turnFactorPitch = (float)-event->moveY / (windowHeight * maxMouseMove);

    // Update turnFactors
    turnFactors.x = glm::max<float>(-1.0f, glm::min<float>(1.0f, turnFactors.x + turnFactorYaw));
    turnFactors.y = glm::max<float>(-1.0f, glm::min<float>(1.0f, turnFactors.y + turnFactorPitch));
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

    direction = glm::vec3(tempDirection);
}
