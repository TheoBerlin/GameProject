#pragma once

#include <Engine/Components/Component.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class Hover : public Component
{
public:
    Hover(Entity* host);

    void update(const float& dt);

private:
    // Animation time
    float t;

    glm::vec3 translation;
    // Defines translation animation
    const glm::vec3 maxTranslation = {0.0f, 0.17f, 0.0f};

    // Yaw, pitch, roll
    glm::vec3 rotation;
    // Defines rotation animation
    const glm::vec3 maxRotation = {0.0f, 0.0f, glm::quarter_pi<float>()/4.0f};
};
