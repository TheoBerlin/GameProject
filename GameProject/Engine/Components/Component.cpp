#include "Component.h"

#include <Engine/Entity/Entity.h>

Component::Component(Entity* parentEntity, const std::string & tagName)
{
	this->tagName = tagName;

	parentEntity->addComponent(this);
	this->setHost(parentEntity);
}

Component::~Component()
{
}

std::string Component::getName()
{
	return this->tagName;
}

Entity * Component::getHost()
{
	return this->host;
}

void Component::setHost(Entity* entity)
{
	this->host = entity;
}

void Component::init()
{
}
