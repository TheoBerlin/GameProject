#pragma once

#include <string>
#include <unordered_map>

#include "../Components/Component.h"
#include "EntityMatrix.h"

#include "../AssetManagement/Model.h"

class Entity
{
public:
	Entity(const glm::vec3& forward = { 1.0f, 0.0f, 0.0f });
	~Entity();

	void update(const float dt);

	bool addComponent(Component* component);
	bool removeComponent(const std::string& componentName);

	void setModel(Model* model);
	Model* getModel();

	void setName(const std::string& name);
	const std::string getName();
	EntityMatrix* getMatrix();

private:
	std::unordered_map<std::string, Component*> components;

	std::string name;
	EntityMatrix matrix;

	Model* model;
};

