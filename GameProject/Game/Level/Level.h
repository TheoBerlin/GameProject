#pragma once

#include <Engine/Entity/EntityManager.h>

class Level
{
public:
	Level();
	~Level();

private:
	EntityManager* entityManager;

};