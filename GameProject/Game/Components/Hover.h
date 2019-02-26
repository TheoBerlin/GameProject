#pragma once

#include <Engine/Components/Component.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>

class Hover : public Component
{
public:
    Hover(Entity* host);

    void update(const float& dt);

    // Resets hover animation
    void reset();

private:
    // Animation time
    float t;

    // Defines translation animation
    const glm::vec3 translationAnimation = {0.0f, 0.17f, 0.0f};
    glm::vec3 beginPos, endPos;

    // Defines rotation animation (yaw, pitch, roll)
    const glm::vec3 rotationAnimation = {0.0f, 0.0f, glm::quarter_pi<float>()/4.0f};
    glm::quat beginRotation;
    glm::vec3 positiveRotation, negativeRotation;
};
