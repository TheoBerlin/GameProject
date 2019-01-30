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

void Component::setHost(Entity* entity)
{
	this->host = entity;
}
