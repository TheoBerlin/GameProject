#pragma once

#include <Engine/Components/Component.h>
#include <Engine/Entity/EntityMatrix.h>
#include <Engine/Events/EventBus.h>
#include <Utils/Logger.h>
#include <Engine/Rendering/Display.h>

class Entity;

class ArrowGuider : public Component
{
public:
    ArrowGuider(Entity* parentEntity, EntityMatrix startingTransform, glm::vec3 startingDirection,
    float movementSpeed = 0.2f, float maxTurnSpeed = glm::quarter_pi<float>() / 2.0f);
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

private:
    // Use turn factors to turn
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
};
