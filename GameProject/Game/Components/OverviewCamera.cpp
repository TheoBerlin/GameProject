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

    // Set the forward vector to be horizontal, then pitch it
    Transform* transform = host->getTransform();

    glm::vec3 forward = transform->getForward();
    glm::vec3 horizontalForward = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));

    transform->setForward(horizontalForward);
    host->getTransform()->rotate(0.0f, pitch, 0.0f);

    glm::vec3 newForward = transform->getForward();

    EventBus::get().subscribe(this, &OverviewCamera::handleMouseMove);
    EventBus::get().subscribe(this, &OverviewCamera::handleKeyInput);
}

OverviewCamera::~OverviewCamera()
{
    EventBus::get().unsubscribe(this, &OverviewCamera::handleMouseMove);
    EventBus::get().unsubscribe(this, &OverviewCamera::handleKeyInput);
}

void OverviewCamera::update(const float& dt)
{
    Transform* transform = host->getTransform();

    glm::vec3 forward = transform->getForward();

    // Move camera horizontally
    glm::vec3 newPosition = transform->getPosition();

    glm::vec3 horizontalForward = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));

    if (this->pressedKeys[GLFW_KEY_W])
		newPosition += dt * horizontalForward * maxMovementSpeed;
	if (this->pressedKeys[GLFW_KEY_A])
		newPosition += dt * -transform->getRight() * maxMovementSpeed;
	if (this->pressedKeys[GLFW_KEY_D])
		newPosition += dt * transform->getRight() * maxMovementSpeed;
	if (this->pressedKeys[GLFW_KEY_S])
		newPosition += dt * -horizontalForward * maxMovementSpeed;
	if (this->pressedKeys[GLFW_KEY_SPACE])
		newPosition += dt * GLOBAL_UP_VECTOR * maxMovementSpeed;
	if (this->pressedKeys[GLFW_KEY_LEFT_CONTROL])
		newPosition += dt * -GLOBAL_UP_VECTOR * maxMovementSpeed;

    transform->setPosition(newPosition);

    // Apply mouse movement
    if (std::abs(mouseMoveX) > FLT_EPSILON * 10.0f) {
        // Rotate around a point in front of the camera
        float rotationAngle = -mouseMoveX * mouseSensitivity * dt;

        glm::vec3 focusPoint = transform->getPosition() + forward * rotationDistance;

        // Reposition then rotate camera
        transform->rotate(glm::vec3(0.0f, rotationAngle, 0.0f), focusPoint);

        transform->rotate(rotationAngle, 0.0f);

        mouseMoveX = 0.0f;
    }
}

void OverviewCamera::handleMouseMove(MouseMoveEvent* event)
{
    mouseMoveX += (float)event->moveX - prevMousePosX;

    prevMousePosX = (float)event->moveX;
}

void OverviewCamera::handleKeyInput(KeyEvent* event)
{
    if (event->action == GLFW_PRESS) {
		this->pressedKeys[event->key] = true;
    } else if (event->action == GLFW_RELEASE) {
		this->pressedKeys[event->key] = false;
    }
}
