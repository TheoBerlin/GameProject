#include "OverviewCamera.h"

#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Display.h>
#include <Utils/Logger.h>
#include <Utils/Settings.h>

OverviewCamera::OverviewCamera(Entity* host)
    :Component(host, "OverviewCamera")
{
    prevMousePosX = 0.0f;
    mouseMoveX = 0.0f;

    mouseSensitivity = Settings::get().getMouseSensitivity();

    rotateFactor = 0.0f;

    windowHeight = Display::get().getHeight();

    // Set the forward vector to be horizontal, then pitch it
    Transform* transform = host->getTransform();

    // Set the desired pitch
    glm::vec3 forward = transform->getForward();
    glm::vec3 horizontalForward = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));

    float currentPitch = std::acosf(glm::dot(horizontalForward, forward));

    currentPitch = (forward.y > 0.0f) ? currentPitch : -currentPitch;

    float deltaPitch = pitch - currentPitch;

    transform->rotate(0.0f, deltaPitch);

    // Lock mouse and get mouse position
    glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    double* cursorPos = new double;
    glfwGetCursorPos(Display::get().getWindowPtr(), cursorPos, nullptr);

    this->prevMousePosX = (float)*cursorPos;

    delete cursorPos;

    EventBus::get().subscribe(this, &OverviewCamera::handleMouseMove);
    EventBus::get().subscribe(this, &OverviewCamera::handleKeyInput);
    EventBus::get().subscribe(this, &OverviewCamera::handleWindowResize);
}

OverviewCamera::~OverviewCamera()
{
    EventBus::get().unsubscribe(this, &OverviewCamera::handleMouseMove);
    EventBus::get().unsubscribe(this, &OverviewCamera::handleKeyInput);
    EventBus::get().unsubscribe(this, &OverviewCamera::handleWindowResize);
}

void OverviewCamera::update(const float& dt)
{
    Transform* transform = host->getTransform();

    glm::vec3 forward = transform->getForward();
    glm::vec3 right = transform->getRight();

    // Update move factors
    moveFactors /= 1.0f + moveFactorFalloff * dt;

    if (this->pressedKeys[GLFW_KEY_W])
		moveFactors.z = 1.0f;
	if (this->pressedKeys[GLFW_KEY_A])
		moveFactors.x = -1.0f;
	if (this->pressedKeys[GLFW_KEY_D])
		moveFactors.x = 1.0f;
	if (this->pressedKeys[GLFW_KEY_S])
		moveFactors.z = -1.0f;
	if (this->pressedKeys[GLFW_KEY_SPACE])
		moveFactors.y = 1.0f;
	if (this->pressedKeys[GLFW_KEY_LEFT_CONTROL])
		moveFactors.y = -1.0f;

    // Keep the length of moveFactors with [0,1]
    float moveFactorsLength = glm::length(moveFactors);

    if (moveFactorsLength > 1.0f) {
        moveFactors /= moveFactorsLength;
    }

    // Apply movement
    glm::vec3 horizontalForward = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));
    glm::vec3 horizontalRight = glm::cross(horizontalForward, GLOBAL_UP_VECTOR);

    glm::vec3 move = horizontalRight * moveFactors.x + GLOBAL_UP_VECTOR * moveFactors.y +
    horizontalForward * moveFactors.z;

    move *= maxMovementSpeed * dt;

    transform->translate(move);

    // Decrease rotation factor over time
    rotateFactor /= 1.0f + rotateFactorFalloff * dt;

    // Apply mouse movement
    applyRotation(dt);
}

void OverviewCamera::handleMouseMove(MouseMoveEvent* event)
{
    mouseMoveX += (float)event->moveX - prevMousePosX;

    prevMousePosX = (float)event->moveX;

    // Divide by window height to separate turn speed from screen resolution
    rotateFactor += mouseSensitivity * mouseMoveX / windowHeight;

    // Keep rotateFactor within [-1,1]
    rotateFactor = glm::clamp(rotateFactor, -1.0f, 1.0f);
}

void OverviewCamera::handleKeyInput(KeyEvent* event)
{
    if (event->action == GLFW_PRESS) {
		this->pressedKeys[event->key] = true;
    } else if (event->action == GLFW_RELEASE) {
		this->pressedKeys[event->key] = false;
    }
}

void OverviewCamera::handleWindowResize(WindowResizeEvent* event)
{
    windowHeight = event->height;
}

void OverviewCamera::applyRotation(const float& dt)
{
    if (std::abs(rotateFactor) < FLT_EPSILON * 10.0f) {
        return;
    }

    Transform* transform = host->getTransform();

    // Rotate around a point in front of the camera
    float rotationAngle = -rotateFactor * maxRotation * dt;

    glm::vec3 focusPoint = transform->getPosition() + transform->getForward() * rotationDistance;

    // Reposition then rotate camera
    transform->rotate(glm::vec3(0.0f, rotationAngle, 0.0f), focusPoint);

    transform->rotate(rotationAngle, 0.0f);

    mouseMoveX = 0.0f;
}
