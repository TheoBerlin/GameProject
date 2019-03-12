#include "Component.h"

#include <Engine/Entity/Entity.h>
#include "Utils/Logger.h"

Component::Component(Entity* parentEntity, const std::string & tagName)
{
	this->tagName = tagName;

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
	// Detach component from old host
	if (this->host != nullptr) {
		this->host->detachComponent(this->tagName);

		this->host = nullptr;
	}

	// Set new host
	if (entity->addComponent(this)) {
		this->host = entity;
	} else {
		LOG_WARNING("Component already exists on entity %s", this->tagName.c_str());
	}
}

void Component::init()
{
}
