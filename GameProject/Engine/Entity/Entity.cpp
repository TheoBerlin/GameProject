#include "Entity.h"

Entity::Entity(const glm::vec3& forward) : model(nullptr)
{
	transform.setForward(forward);

	this->renderingGroupIndex = -1;
	this->hasMoved = false;
}

Entity::~Entity()
{
	for (auto& it : this->components) {
		delete it.second;
		it.second = nullptr;
	}

	this->components.clear();
	/*
		Remove entity from models rendering group
	*/
	if (this->model != nullptr) {
		this->model->removeEntity(this->renderingGroupIndex);
	}
}

void Entity::update(const float dt)
{
	this->hasMoved = false;

	for (auto& it : this->components)
		it.second->update(dt);

	/*
		Updates vertex buffer of model if a component has moved the entity
	*/
	if (this->model && this->hasMoved)
		this->model->updateInstancingSpecificData(this->renderingGroupIndex);
}

bool Entity::addComponent(Component * component)
{
	auto ite = this->components.find(component->getName());

	if (ite == this->components.end()) {
		this->components[component->getName()] = component;
		component->setHost(this);
		return true;
	}

	return false;
}

bool Entity::removeComponent(const std::string& componentName)
{
	auto ite = this->components.find(componentName);

	if (ite != this->components.end()) {
		delete ite->second;
		this->components.erase(ite);
		return true;
	}

	return false;
}

Component* Entity::getComponent(const std::string& componentName)
{
	// Returns nullptr if component is not found
	auto elem = this->components.find(componentName);
	if (elem != this->components.end())
		return elem->second;
	else
		return nullptr;
}

void Entity::setModel(Model * model)
{
	/*
		Remove itself from old model, Complexity: Linear on the entities after the entity deleted 
	*/
	if (this->renderingGroupIndex != -1)
		this->model->removeEntity(this->renderingGroupIndex);

	this->renderingGroupIndex = model->addEntity(this);
	this->model = model;
	this->model->updateInstancingData();
}

void Entity::setRenderingGroupIndex(unsigned index)
{
	this->renderingGroupIndex = index;
}

unsigned Entity::getRenderingGroupIndex()
{
	return this->renderingGroupIndex;
}

Model * Entity::getModel()
{
	return this->model;
}

void Entity::hasMovedThisFrame()
{
	this->hasMoved = true;
}

void Entity::setName(const std::string & name)
{
	this->name = name;
}

const std::string Entity::getName()
{
	return this->name;
}

Transform * Entity::getTransform()
{
	return &transform;
}
