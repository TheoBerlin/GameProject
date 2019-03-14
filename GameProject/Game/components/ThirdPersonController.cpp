#include "ThirdPersonController.h"

#include <Engine/Entity/Entity.h>
#include <Engine/Events/EventBus.h>
#include <Utils/Logger.h>
#include <Utils/Settings.h>
#include <glm/gtx/vector_angle.hpp>

ThirdPersonController::ThirdPersonController(Entity* host)
	:Component(host, "ThirdPersonController")
{
	this->windowHeight = Settings::get().getScreenHeight();
	this->mouseSens = Settings::get().getMouseSensitivity();

	this->mouseMove = { 0, 0 };
	this->zoomFactor = 0.0f;

	// Handle scroll events regardless of whether mouse movement is being handled
	EventBus::get().subscribe(this, &ThirdPersonController::handleMouseScroll);

	// Handle window resizes
	EventBus::get().subscribe(this, &ThirdPersonController::handleWindowResize);

	// Do not handle mouse movement by default

	// Get camera
	Component* tmp = host->getComponent("Camera");

	if (tmp) {
		this->camera = dynamic_cast<Camera*>(tmp);
	}
	else {
		LOG_WARNING("Could not find Camera component");

		return;
	}

	// Decouple camera so that it may float around the entity separately
	this->camera->decouple();

	Transform* transform = host->getTransform();

	// Set beginning camera forward
	glm::vec3 entityPos = transform->getPosition();

	glm::vec3 camPos = camera->getPosition();

	glm::vec3 camToEntity = entityPos - camPos;

	this->distToEntity = glm::length(camPos - entityPos);

	glm::vec3 camForward;

	glm::vec3 entityForward = transform->getForward();

	// If the camera was not decoupled from its entity to begin with, set the forward to the entity's forward
	if (distToEntity < 0.001f) {
		camForward = entityForward;
	}
	else {
		camForward = camToEntity / distToEntity;
	}

	camera->setForward(camForward);

	// Get host entity yaw
	this->hostYaw = transform->getYaw();

	// Calculate current pitch
	glm::vec3 horizCamForward = glm::normalize(glm::vec3(camForward.x, 0.0f, camForward.z));
	glm::vec3 horizEntityForward = glm::normalize(glm::vec3(entityForward.x, 0.0f, camForward.z));

	this->currentPitch = glm::orientedAngle(horizCamForward, camForward, camera->getRight());

	// Clamp camera's distance to the host entity
	if (distToEntity < minDistEntity) {
		camera->setPosition(entityPos - camForward * minDistEntity);

		distToEntity = minDistEntity;
	}
	else if (distToEntity > maxDistEntity) {
		camera->setPosition(entityPos - camForward * maxDistEntity);

		distToEntity = maxDistEntity;
	}
}

ThirdPersonController::~ThirdPersonController()
{
	disableMouse();

	EventBus::get().unsubscribe(this, &ThirdPersonController::handleMouseScroll);
}

void ThirdPersonController::update(const float& dt)
{
	// The controller should not do anything without a camera
	if (!camera) {
		return;
	}

	// Sync yaw with host entity
	Transform* transform = host->getTransform();

	float newHostYaw = transform->getYaw();

	float yawDiff = newHostYaw - hostYaw;

	hostYaw = newHostYaw;

	// Apply mouse movement and yaw difference
	// Yaw
	float mouseYaw = 0.0f;

	if (mouseMove.x != 0) {
		mouseYaw += -(float)mouseMove.x * dt * this->mouseSens;

		// Reset per-frame mouse move
		mouseMove.x = 0;
	}

	// Apply yaw
	glm::quat rotationQuat = { 1.0f, 0.0f, 0.0f, 0.0f };

	rotationQuat = glm::rotate(rotationQuat, mouseYaw + yawDiff, GLOBAL_UP_VECTOR);

	glm::vec3 camForward = camera->getForward();

	camForward = rotationQuat * camForward;

	camera->setForward(camForward);

	// Calculate and apply pitch
	if (mouseMove.y != 0) {
		float pitch = (float)-mouseMove.y * dt * this->mouseSens;

		float newPitch = pitch + currentPitch;

		// Limit pitch
		if (newPitch > maxPitch) {
			pitch = maxPitch - currentPitch;
		}
		else if (newPitch < -maxPitch) {
			pitch = -maxPitch - currentPitch;
		}

		rotationQuat = { 1.0f, 0.0f, 0.0f, 0.0f };

		rotationQuat = glm::rotate(rotationQuat, pitch, camera->getRight());

		camForward = rotationQuat * camForward;

		camera->setForward(camForward);

		currentPitch += pitch;

		// Reset per-frame mouse move
		mouseMove.y = 0;
	}

	// Apply zoom factor
	distToEntity += zoomFactor * maxZoomSpeed * dt;

	distToEntity = glm::clamp(distToEntity, minDistEntity, maxDistEntity);

	zoomFactor /= 1.0f + zoomFactorFalloff * dt;

	// Reposition camera
	camera->setPosition(transform->getPosition() - camForward * distToEntity);
}

void ThirdPersonController::enableMouse()
{
	EventBus::get().subscribe(this, &ThirdPersonController::handleMouseMove);
}

void ThirdPersonController::disableMouse()
{
	EventBus::get().unsubscribe(this, &ThirdPersonController::handleMouseMove);
}

void ThirdPersonController::handleMouseMove(MouseMoveEvent* event)
{
	this->mouseMove += glm::ivec2(event->deltaX, event->deltaY);
}

void ThirdPersonController::handleMouseScroll(MouseScrollEvent* event)
{
	this->zoomFactor += (float)-event->yoffset;

	this->zoomFactor = glm::clamp(this->zoomFactor, -1.0f, 1.0f);
}

void ThirdPersonController::handleWindowResize(WindowResizeEvent* event)
{
	this->windowHeight = event->height;
}