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
    if (isGuiding) {
        EventBus::get().unsubscribe(this, &ArrowGuider::handleMouseMove);
    }
}

void ArrowGuider::update(const float& dt)
{
    if (!isGuiding) {
        // Do nothing if the guider is disabled
        return;
    }

    glm::vec3 direction = host->getTransform()->getForward();

    // Update turn factors (proportionally slow down turning)
    turnFactors.x /= 1.0f + turnFactorFalloff * dt;
    turnFactors.y /= 1.0f + turnFactorFalloff * dt;

    applyTurn(dt);

    // Update position storing frequency based on turning factors
    float turnFactorsLength = glm::length(turnFactors);
    posStoreFrequency = minStoreFrequency + (maxStoreFrequency - minStoreFrequency) * turnFactorsLength;

    // Update position store timer
    posStoreTimer += dt;

    if (posStoreTimer > 1.0f/posStoreFrequency) {
        // Store position and reset timer
        std::fmod(posStoreTimer, posStoreFrequency);

        storedPositions.push_back(host->getTransform()->getPosition());
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

    // Update arrow position
    Transform* transform = host->getTransform();

    glm::vec3 currentPos = transform->getPosition();
	glm::vec3 newPos = currentPos + direction * movementSpeed * dt;

    transform->setPosition(newPos);
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

    // Clear previous path and store starting position
    storedPositions.clear();
    storedPositions.push_back(host->getTransform()->getPosition());

    // Set camera settings
    arrowCamera->setFOV(minFOV);
    arrowCamera->setOffset(minCamOffset);

    // Lock cursor
    glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Get cursor position
    double* cursorX = new double, *cursorY = new double;
    glfwGetCursorPos(Display::get().getWindowPtr(), cursorX, cursorY);

    mousePos = glm::vec2((float)*cursorX, (float)*cursorY);

    delete cursorX;
    delete cursorY;

    EventBus::get().subscribe(this, &ArrowGuider::handleMouseMove);
}

void ArrowGuider::stopGuiding()
{
    isGuiding = false;

    EventBus::get().unsubscribe(this, &ArrowGuider::handleMouseMove);

    turnFactors.x = 0.0f;
    turnFactors.y = 0.0f;

    posStoreTimer = 0.0f;

    // Unlock cursor
    glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void ArrowGuider::handleMouseMove(MouseMoveEvent* event)
{
    // Calculate relative mouse position
    float moveX = (float)event->moveX - mousePos.x;
    float moveY = (float)event->moveY - mousePos.y;

    mousePos.x = (float)event->moveX;
    mousePos.y = (float)event->moveY;

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

std::vector<glm::vec3>& ArrowGuider::getStoredPositions()
{
    return storedPositions;
}

float ArrowGuider::getTurningSpeed()
{
    return glm::length(turnFactors) * maxTurnSpeed;
}

void ArrowGuider::applyTurn(const float& dt)
{
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