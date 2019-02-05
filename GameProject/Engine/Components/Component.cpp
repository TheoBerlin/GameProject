#include "Component.h"

Component::Component(const std::string & tagName)
{
	this->tagName = tagName;
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
