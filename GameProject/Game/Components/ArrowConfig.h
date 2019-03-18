#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// Settings for the player arrow, some needed to be accessed from outside the arrow guider class
namespace Arrow {
    const glm::vec3 minCamOffset = {0.0f, 0.15f, -0.75f}, maxCamOffset = {0.0f, 0.15f, -1.0f};

    const float minFOV = 75.0f, maxFOV = 90.0f;

    const float minSpeed = 3.2f, maxSpeed = 5.0f;

    const float minTurnSpeed = glm::half_pi<float>() * 1.1f, maxTurnSpeed = glm::half_pi<float>() * 1.6f;
}
