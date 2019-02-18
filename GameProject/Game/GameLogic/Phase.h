#pragma once

#include <Game/Level/Level.h>
#include <Engine/Entity/Entity.h>

class Phase
{
public:
    Phase(const Level& level);
    ~Phase();

protected:
    Entity* playerCamera;
    Entity* playerArrow;

    Level level;
};
