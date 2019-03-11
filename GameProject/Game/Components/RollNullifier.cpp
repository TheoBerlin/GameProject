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

    float roll = transform->getRoll();

    // Calculate roll delta
	float rollDelta = roll * (rollNullifyFactor - 1.0f) * dt;

    // Limit roll in case of large dt
    if (std::abs(rollDelta) > std::abs(roll)) {
        rollDelta = -roll;
    }

    host->getTransform()->rotate(0.0f, 0.0f, rollDelta);
}
