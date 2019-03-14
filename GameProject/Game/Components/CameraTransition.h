#pragma once

#include <Engine/Components/Component.h>
#include <Game/Components/ComponentResources.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

class Camera;

class CameraTransition : public Component
{
public:
    CameraTransition(Entity* host);
    CameraTransition(Entity* host, const glm::vec3& newPos, const glm::vec3& newForward, float newFOV, float transitionLength);
    ~CameraTransition();

    void setDestination(const glm::vec3& newPos, const glm::vec3& newForward, float newFOV, float transitionLength);
    void setPath(const std::vector<KeyPoint>& path, const glm::vec3& newForward, float newFOV);

    void update(const float& dt);
private:
    void catmullRomMove();

    // Quaternions for the default forward and final forward directions
    glm::quat beginQuat, endQuat;

    glm::vec3 defaultForward;

    float beginFOV, endFOV;

    Camera* entityCam;

    float transitionTime;

    // Timestamp at previous path point
    float beginT;

    bool isTransitioning;

    std::vector<KeyPoint> path;

    unsigned int pathIndex;
};
