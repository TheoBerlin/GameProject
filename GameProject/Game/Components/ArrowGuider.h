#pragma once

#include <Engine/Components/Camera.h>
#include <Engine/Components/Component.h>
#include <Engine/Entity/Transform.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Display.h>
#include <Game/components/ComponentResources.h>
#include <vector>
#include <cmath>
#include <glm/gtc/quaternion.hpp>

class Entity;

class ArrowGuider : public Component
{
public:
    ArrowGuider(Entity* parentEntity, glm::vec3 minCamOffset, float minFOV, float movementSpeed = 3.0f, float maxTurnSpeed = glm::half_pi<float>());
    ~ArrowGuider();

    void update(const float& dt);

    // Enables aim but not movement
    void startAiming();
    // Disables aim but not movement
    void stopAiming();

    // Enables both aim and movement
    void startGuiding();
    // Disables both aim and movement
    void stopGuiding(float flightTime);

    // Overwrites the last keypoint with the current time and position
    void saveKeyPoint(float flightTime);

    // Event handlers
    void handleMouseMove(MouseMoveEvent* event);
    void handleWindowResize(WindowResizeEvent* event);
	void handleKeyEvent(KeyEvent* event);

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

	float maxSpeedOffset;
	float movementSpeed;
	float maxSpeedIncrease;
	float minSpeedDecrease;
	float acceleration;
	bool isAccelerating;
	
	int windowHeight;
    // Max turn speed measured in radians
    float maxTurnSpeed;
    // Mouse movement, relative to window height, required to reach max turn speed
    const float maxMouseMove = 0.5f;

    // Used to create prolonged turns, i.e. slowly turning the arrow over time
    glm::vec2 turnFactors;
    // Defines how fast turn factors fall off over time (per second)
    const float turnFactorFalloff = 1.0f;

    glm::vec3 direction;

    // Disables and enables functionality within the guider
    bool isAiming, isGuiding;

    // Frequency at which the position is stored
    float posStoreFrequency;
    const float minStoreFrequency = 0.7f, maxStoreFrequency = 1.2f;
    const float maxStoreFrequencyDelta = 0.15f;

    // Time since position was stored
    float posStoreTimer;
    float flightTime;
    std::vector<KeyPoint> path;

    // Allow copying over the last key point in the path
    // Used to avoid collision keypoints being written over each other
    bool allowKeypointOverride;

    // The pointer is retrieved when startGuiding() is called
    Camera* arrowCamera;

    // Camera settings
    glm::vec3 minCamOffset;
    glm::vec3 maxCamOffset;
    // Max offset change in forward direction per second
    const float offsetChangeMax = 0.35f;

    float minFOV;
    // Max FOV and max FOV change per second
    const float maxFOV = 90.0f, FOVChangeMax = 15.0f;

    float currentPitch;
    const float maxPitch = glm::half_pi<float>() - 0.01f;
};
