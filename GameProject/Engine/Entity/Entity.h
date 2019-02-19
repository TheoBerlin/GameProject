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
	void removeAllComponents();
	// Used by components to get neighboring components
	Component* getComponent(const std::string& componentName);

	void setModel(Model* model);
	Model* getModel();

	/*
		Attaches entity to model, entity attaches itself to renderingGroup of model if models is set
	*/
	void attachToModel();

	/*
		Detaches entity from model, entity keeps models but stops being drawn
	*/
	void detachFromModel();
		
	void setRenderingGroupIndex(unsigned index);
	unsigned getRenderingGroupIndex();

	void setName(const std::string& name);
	const std::string getName();
	Transform* getTransform();

private:
	std::unordered_map<std::string, Component*> components;

	std::string name;
	Transform transform;

	/*
		Used to identify position in models renderingGroup, so entites can remove themselves from being rendered. -1 indicates not model attached
	*/
	int renderingGroupIndex;
	Model* model;
};
