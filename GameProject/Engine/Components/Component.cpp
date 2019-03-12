#include "Component.h"

#include <Engine/Entity/Entity.h>
#include "Utils/Logger.h"

Component::Component(Entity* parentEntity, const std::string & tagName)
{
	this->tagName = tagName;

	if (parentEntity->addComponent(this)) 
		this->setHost(parentEntity);
	else {
		LOG_WARNING("Component already exists on entity");
	}
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
