#pragma once

#include <Engine/Components/Component.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Events/Events.h>
#include <Engine/Entity/Entity.h>
#include <Utils/Settings.h>
#include <glm/glm.hpp>

class OverviewCamera : public Component
{
public:
    OverviewCamera(Entity* host);
    ~OverviewCamera();

    void update(const float& dt);

private:
    void handleMouseMove(MouseMoveEvent* event);

    float mouseSensitivity;

    // Refreshed each update call
    float prevMousePosX;
    float mouseMoveX;

    // Distance to the point of rotation
    const float rotationDistance = 20.0f;

    const float pitch = -glm::quarter_pi<float>();
};
