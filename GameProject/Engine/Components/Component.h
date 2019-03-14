#pragma once

#include <string>

class Entity;

class Component
{
public:
	Component(Entity* parentEntity, const std::string& tagName);
	virtual ~Component();

	std::string getName();
	Entity* getHost();
	void setHost(Entity* entity);

	virtual void init();
	virtual void update(const float& dt) = 0;

protected:
	Entity* host;

	std::string tagName;
};