#pragma once

#include <GLM/glm.hpp>

struct KeyPoint {
    KeyPoint() {};
    KeyPoint(glm::vec3 Position, glm::vec3 UpVector, float time) : Position(Position), UpVector(UpVector), t(time) {};

    glm::vec3 Position;
	glm::vec3 UpVector;
    float t;
};
