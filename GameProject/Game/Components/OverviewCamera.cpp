#include "OverviewCamera.h"

#include <Utils/Logger.h>

OverviewCamera::OverviewCamera(Entity* host)
    :Component(host, "OverviewCamera")
{
    //EventBus::get().subscribe(this, &OverviewCamera::handleMouseMove);

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

    LOG_INFO("(%f,%f,%f)", newForward.x, newForward.y, newForward.z);
}

OverviewCamera::~OverviewCamera()
{
    //EventBus::get().unsubscribe(this, &OverviewCamera::handleMouseMove);
}

void OverviewCamera::update(const float& dt)
{
    /*
    // Rotate around a point in front of the camera
    Transform* transform = host->getTransform();
    glm::vec3 focusPoint = transform->getPosition() + transform->getForward() * rotationDistance;

    float rotationAngle = -mouseMoveX * mouseSensitivity * dt;
    mouseMoveX = 0.0f;

    // Reposition then rotate camera
    transform->rotate(GLOBAL_UP_VECTOR * rotationAngle, focusPoint);
    transform->setForward(focusPoint);*/
}

void OverviewCamera::handleMouseMove(MouseMoveEvent* event)
{
    mouseMoveX += (float)event->moveX - prevMousePosX;

    prevMousePosX = (float)event->moveX;
}
