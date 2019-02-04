#pragma once

#include <string>

class Entity;

class Component
{
public:
	Component(Entity* parentEntity, const std::string& tagName);
	~Component();

	std::string getName();
	void setHost(Entity* entity);

	virtual void init();
	virtual void update(const float& dt) = 0;

protected:
	Entity* host;

	std::string tagName;
};
