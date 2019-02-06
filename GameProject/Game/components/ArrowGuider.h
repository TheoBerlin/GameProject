#pragma once

#include <Engine/Components/Camera.h>
#include <Engine/Components/Component.h>
#include <Engine/Entity/Transform.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Display.h>
#include <Utils/Logger.h>
#include <vector>

class Entity;

class ArrowGuider : public Component
{
public:
    ArrowGuider(Entity* parentEntity, float movementSpeed = 0.2f, float maxTurnSpeed = glm::quarter_pi<float>() / 2.0f);
    ~ArrowGuider();

    void update(const float& dt);

    void startGuiding();
    void stopGuiding();

    // Event handlers
    void handleMouseMove(MouseMoveEvent* event);
    void handleWindowResize(WindowResizeEvent* event);

    float getMaxTurnSpeed();
    void setMaxTurnSpeed(const float maxTurnSpeed);

    glm::vec3 getDirection();
    void setDirection(const glm::vec3 direction);

    glm::vec3 getPosition();
    void setPosition(const glm::vec3 position);

    float getMovementSpeed();
    void setMovementSpeed(const float speed);

    float getPosStoreFrequency();
    std::vector<glm::vec3>& getStoredPositions();

    // Redirection measured in radians per second
    float getTurningSpeed();

private:
    // Use turn factors to update direction
    void applyTurn();

    float movementSpeed;
    int windowHeight;
    // Max turn speed measured in radians
    float maxTurnSpeed;
    // Mouse movement, relative to window height, required to reach max turn speed
    const float maxMouseMove = 0.7f;

    // Used to create prolonged turns, i.e. slowly turning the arrow over time
    glm::vec2 turnFactors;
    // Defines how fast turn factors fall off over time (per second)
    const float turnFactorFalloff = 1.0f;

    glm::vec3 direction;

    // Disables and enables the guider
    bool isGuiding;

    // Frequency at which the position is stored
    float posStoreFrequency;
    const float minStoreFrequency = 5.0f, maxStoreFrequency = 20.0f;
    // Time since position was stored
    float posStoreTimer;
    std::vector<glm::vec3> storedPositions;

    // The pointer is retrieved when startGuiding() is called
    Camera* arrowCamera;

    // Camera settings
    const glm::vec3 minCamOffset = glm::vec3(0.0f, 0.1f, 0.2f), maxCamOffset = glm::vec3(0.0f, 0.1f, 0.8f);
    const float offsetChangeMax = 0.5f;

    const float minFOV = 75.0f, maxFOV = 95.0f;
    const float FOVChangeMax = 20.0f;
};
