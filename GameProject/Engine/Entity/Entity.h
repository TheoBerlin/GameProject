#pragma once

#include <string>
#include <unordered_map>

#include "../Components/Component.h"
#include "Transform.h"

#include "../AssetManagement/Model.h"

namespace reactphysics3d {
	class CollisionBody;
}

class Entity
{
public:
	Entity(const glm::vec3& forward = { 0.0f, 0.0f, -1.0f });
	~Entity();

	void update(const float dt);

	bool addComponent(Component* component);
	// Deletes and unlists component
	bool removeComponent(const std::string& componentName);
	void removeAllComponents();
	// Unlists component
	bool detachComponent(const std::string& componentName);
	/*
		Get component by name for the given entity
		Returns:
			Component pointer if found, nullptr if not.
	*/
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
	const std::string& getName();
	Transform* getTransform();
	Transform* getPausedTransform();

	void pauseModelTransform();
	void unpauseModelTransform();

	bool isTransformSeparated() const;

	void setCollisionBody(reactphysics3d::CollisionBody* body);
	reactphysics3d::CollisionBody* getCollisionBody() const;

private:
	std::unordered_map<std::string, Component*> components;

	std::string name;
	Transform transform;
	Transform pausedTransform;

	bool hasSeparatedTransforms;

	/*
		Used to identify position in models renderingGroup, so entites can remove themselves from being rendered. -1 indicates not model attached
	*/
	int renderingGroupIndex;
	Model* model;
	reactphysics3d::CollisionBody* body;
};