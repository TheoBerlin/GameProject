#pragma once

#include <string>
#include <unordered_map>

#include "../Components/Component.h"
#include "Transform.h"

#include "../AssetManagement/Model.h"

class Entity
{
public:
	Entity(const glm::vec3& forward = { 0.0f, 0.0f, -1.0f });
	~Entity();

	void update(const float dt);

	bool addComponent(Component* component);
	bool removeComponent(const std::string& componentName);

	void setModel(Model* model);
	Model* getModel();

	void setName(const std::string& name);
	const std::string getName();
	Transform* getTransform();

private:
	std::unordered_map<std::string, Component*> components;

	std::string name;
	Transform transform;

	Model* model;
};

