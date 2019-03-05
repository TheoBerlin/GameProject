#pragma once

#include <GLM/glm.hpp>

struct KeyPoint {
    KeyPoint() {};
    KeyPoint(glm::vec3 Position, float time) : Position(Position), t(time) {};

    glm::vec3 Position;
    float t;
};
