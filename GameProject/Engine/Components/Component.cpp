#include "Component.h"

#include <Engine/Entity/Entity.h>

Component::Component(Entity* parentEntity, const std::string & tagName)
{
	parentEntity->addComponent(this);
	this->setHost(parentEntity);

	this->tagName = tagName;
}

Component::~Component()
{
}

std::string Component::getName()
{
	return this->tagName;
}

void Component::setHost(Entity* entity)
{
	this->host = entity;
}

void Component::init()
{
}
