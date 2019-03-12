#pragma once

#include <Engine/Components/Component.h>
#include <Engine/Components/Camera.h>
#include <Engine/Events/Events.h>

class ThirdPersonController : public Component
{
public:
    ThirdPersonController(Entity* host);
    ~ThirdPersonController();

    void update(const float& dt);

    void enableMouse();
    void disableMouse();

private:
    void handleMouseMove(MouseMoveEvent* event);
    void handleMouseScroll(MouseScrollEvent* event);

    void handleWindowResize(WindowResizeEvent* event);

    Camera* camera;

    // Variables for applying mouse movement
    unsigned int windowHeight;
    float mouseSens;

    float currentPitch;
    const float maxPitch = glm::half_pi<float>() - 0.01f;

    // Distance between entity and camera
    float distToEntity;
    const float minDistEntity = 0.5f, maxDistEntity = 2.0f;

    // Gradually decreased over time
    float zoomFactor;
    const float maxZoomSpeed = 1.3f, zoomFactorFalloff = 5.0f;

    // Position to rotate around and look at
    glm::vec3 lookAt;

    // Track the yaw of the host entity
    float hostYaw;

    // Per-frame mouse movement, reset each update
    glm::ivec2 mouseMove;

};
