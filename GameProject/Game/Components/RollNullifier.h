#pragma once

/*
    Decreases host entity's roll over time
*/

#include <glm/glm.hpp>
#include <Engine/Components/Component.h>
#include <Engine/Entity/Entity.h>
#include <Utils/Logger.h>

class RollNullifier : public Component
{
public:
    RollNullifier(Entity* host);
    ~RollNullifier();

    void update(const float& dt);

private:
    // Percentage of roll that is nullified per second
    const float rollNullifyFactor = 0.4f;
};
