#pragma once

#include <Engine/Components/Component.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class CameraTransition : public Component
{
public:
    CameraTransition(Entity* host);
    CameraTransition(Entity* host, const glm::vec3& newPos, const glm::vec3& newForward, float transitionLength);
    ~CameraTransition();

    void setDestination(const glm::vec3& newPos, const glm::vec3& newForward, float transitionLength);

    void update(const float& dt);
private:
    // Beginning and end quaternions for the rotation
    glm::quat beginQuat, endQuat;

    glm::vec3 beginPos, endPos;
    glm::vec3 defaultForward;

    float transitionTime, transitionLength;

    bool isTransitioning;
};