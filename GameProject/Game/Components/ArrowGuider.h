#pragma once

#include <Engine/Components/Camera.h>
#include <Engine/Components/Component.h>
#include <Engine/Entity/Transform.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Display.h>
#include <Game/components/ComponentResources.h>
#include <Utils/Logger.h>
#include <vector>
#include <cmath>
#include <glm/gtc/quaternion.hpp>

class Entity;

class ArrowGuider : public Component
{
public:
    ArrowGuider(Entity* parentEntity, float movementSpeed = 3.0f, float maxTurnSpeed = glm::half_pi<float>());
    ~ArrowGuider();

    void update(const float& dt);

    void startGuiding();
    void stopGuiding();

    // Event handlers
    void handleMouseMove(MouseMoveEvent* event);
    void handleWindowResize(WindowResizeEvent* event);

    float getMaxTurnSpeed();
    void setMaxTurnSpeed(const float maxTurnSpeed);

    float getMovementSpeed();
    void setMovementSpeed(const float speed);

    float getPosStoreFrequency();
    std::vector<KeyPoint>& getPath();

    // Redirection measured in radians per second
    float getTurningSpeed();

private:
    // Use turn factors to update direction
    void applyTurn(const float& dt);

    float movementSpeed;
    int windowHeight;
    // Max turn speed measured in radians
    float maxTurnSpeed;
    // Mouse movement, relative to window height, required to reach max turn speed
    const float maxMouseMove = 0.5f;

    glm::vec2 mousePos;

    // Used to create prolonged turns, i.e. slowly turning the arrow over time
    glm::vec2 turnFactors;
    // Defines how fast turn factors fall off over time (per second)
    const float turnFactorFalloff = 1.0f;

    glm::vec3 direction;

    // Disables and enables the guider
    bool isGuiding;

    // Frequency at which the position is stored
    float posStoreFrequency;
    const float minStoreFrequency = 0.5f, maxStoreFrequency = 1.0f;
    const float maxStoreFrequencyDelta = 0.15f;

    // Time since position was stored
    float posStoreTimer;
    float flightTime;
    std::vector<KeyPoint> path;

    // The pointer is retrieved when startGuiding() is called
    Camera* arrowCamera;

    // Camera settings
    const glm::vec3 minCamOffset = glm::vec3(0.0f, 0.3f, -1.0f), maxCamOffset = glm::vec3(0.0f, 0.3f, -1.6f);
    const float offsetChangeMax = 0.35f;

    const float minFOV = 75.0f, maxFOV = 90.0f;
    const float FOVChangeMax = 15.0f;

    float currentPitch;
    const float maxPitch = glm::half_pi<float>() - FLT_EPSILON;
};
