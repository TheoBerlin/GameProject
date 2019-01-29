#include "Entity.h"

Entity::Entity()
{

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
