#include "Entity.h"

Entity::Entity(const glm::vec3& forward)
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
