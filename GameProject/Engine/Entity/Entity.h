#pragma once

#include <string>
#include <unordered_map>

#include "../Components/Component.h"
#include "EntityMatrix.h"

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
	EntityMatrix* getMatrix();

private:
	std::unordered_map<std::string, Component*> components;

	std::string name;
	EntityMatrix matrix;

};

