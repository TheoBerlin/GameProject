#pragma once

#include <string>
#include <unordered_map>

#include "../Components/Component.h"

class Entity
{
public:
	Entity();
	~Entity();

	void update(const float dt);

	bool addComponent(Component* component);
	bool removeComponent(const std::string& componentName);

	void setName(const std::string& name);
	const std::string getName();

private:
	std::unordered_map<std::string, Component*> components;

	std::string name;

};
