#pragma once

#include <GLM/glm.hpp>

struct KeyPoint {
    KeyPoint() {};
    KeyPoint(glm::vec3 Position, glm::vec3 UpVector, float time) : Position(Position), UpVector(UpVector), t(time) {};

    glm::vec3 Position = glm::vec3(0.f);
	glm::vec3 UpVector = glm::vec3(0.f, 1.f, 0.f);
    float t = 0.f;
};
