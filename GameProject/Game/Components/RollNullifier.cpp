#include "RollNullifier.h"

RollNullifier::RollNullifier(Entity* host)
    :Component(host, "RollNullifier")
{
}

RollNullifier::~RollNullifier()
{
}

void RollNullifier::update(const float& dt)
{
    // Calculate roll
	Transform* transform = host->getTransform();
    glm::vec3 rightVec = transform->getRight();

	// Do not proceed if there is no roll
	if (std::abs(rightVec.y) < FLT_EPSILON) {
		return;
	}

    glm::vec3 horizontalRight = glm::normalize(glm::vec3(rightVec.x, 0.0f, rightVec.z));

	float cosRoll = glm::min<float>(glm::dot(rightVec, horizontalRight), 1.0f - FLT_EPSILON);

    float roll = std::acosf(cosRoll);

    roll = (rightVec.y > 0.0f) ? -roll : roll;

    // Calculate roll delta
	float rollDelta = roll * (rollNullifyFactor - 1.0f) * dt;

    // Limit roll in case of large dt
    if (std::abs(rollDelta) > std::abs(roll)) {
        rollDelta = -roll;
    }

    host->getTransform()->rotate(0.0f, 0.0f, rollDelta);
}
