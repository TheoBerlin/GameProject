#include "Level.h"

Level::Level()
{
    entityManager = new EntityManager();
}

Level::~Level()
{
    delete entityManager;
}
