#include "Entity.h"

Entity::Entity(const glm::vec3& forward) : model(nullptr)
{
	matrix.setForward(forward);
}


Entity::~Entity()
{
	for (auto& it : this->components) {
		delete it.second;
		it.second = nullptr;
	}

	this->components.clear();
}

void Entity::update(const float dt)
{
	for (auto& it : this->components)
		it.second->update(dt);
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
	return this->components[componentName];
}

void Entity::setModel(Model * model)
{
	this->model = model;
}

Model * Entity::getModel()
{
	return this->model;
}

void Entity::setName(const std::string & name)
{
	this->name = name;
}

const std::string Entity::getName()
{
	return this->name;
}

EntityMatrix * Entity::getMatrix()
{
	return &matrix;
}
