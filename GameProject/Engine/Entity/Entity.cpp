#include "Entity.h"

int Entity::num = 0;

Entity::Entity()
{
	myNum = num;
	num++;
}


Entity::~Entity()
{
}

void Entity::update(const float dt)
{
}

void Entity::setName(const std::string & name)
{
	this->name = name;
}

const std::string Entity::getName()
{
	return this->name;
}
