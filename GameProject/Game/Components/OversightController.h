#pragma once

#include <Engine/Components/Component.h>
#include <Engine/Events/Events.h>
#include <Engine/Entity/Entity.h>
#include <glm/glm.hpp>
#include <unordered_map>

class OversightController : public Component
{
public:
    OversightController(Entity* host);
    ~OversightController();

    void update(const float& dt);

private:
    void handleMouseMove(MouseMoveEvent* event);
    void handleKeyInput(KeyEvent* event);
    void handleWindowResize(WindowResizeEvent* event);

    void applyRotation(const float& dt);

    // Mouse variables
    // [-1,1] factor decreases over time using falloff factor
    float rotateFactor;
    const float rotateFactorFalloff = 6.0f;

    float mouseSensitivity;

    // Max rotation in radians per second
    const float maxRotation = glm::two_pi<float>() * 2.5f;

    // Scales mouse input
    int windowHeight;

    // Refreshed each update call
    unsigned int prevMousePosX;
    int mouseMoveX;

    // Distance to the focus point
    const float rotationDistance = 6.0f;

    // Track keyboard input
    std::unordered_map<unsigned, bool> pressedKeys;
    /*
        [-1,1] factors determining moving directions and speed factors
        Components are (right, up, forward)
    */
    glm::vec3 moveFactors;
    const float moveFactorFalloff = 9.0f;

    // Max movement speed
    const float maxMovementSpeed = 6.0f;
};
