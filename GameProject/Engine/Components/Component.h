#pragma once

#include <string>

class Entity;

class Component
{
public:
	Component(const std::string& tagName);
	~Component();

	std::string getName();
	void setHost(Entity* entity);

	virtual void init() = 0;
	virtual void update(const float& dt) = 0;

private:
	Entity* host;

	std::string tagName;
};

