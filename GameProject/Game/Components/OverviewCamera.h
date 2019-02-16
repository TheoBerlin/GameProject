#pragma once

#include <Engine/Components/Component.h>
#include <Engine/Events/Events.h>
#include <Engine/Entity/Entity.h>
#include <glm/glm.hpp>
#include <unordered_map>

class OverviewCamera : public Component
{
public:
    OverviewCamera(Entity* host);
    ~OverviewCamera();

    void update(const float& dt);

private:
    void handleMouseMove(MouseMoveEvent* event);
    void handleKeyInput(KeyEvent* event);

    float mouseSensitivity;

    // Refreshed each update call
    float prevMousePosX;
    float mouseMoveX;

    // Distance to the focus point
    const float rotationDistance = 10.0f;
    //glm::vec3 focusPoint;

    const float pitch = -glm::quarter_pi<float>();

    // Track keyboard input
    std::unordered_map<unsigned, bool> pressedKeys;

    const float maxMovementSpeed = 5.0f;
};
