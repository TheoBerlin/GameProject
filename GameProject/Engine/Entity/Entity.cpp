#include "Entity.h"

#include "reactphysics3d/reactphysics3d.h"

Entity::Entity(const glm::vec3& forward) : model(nullptr)
{
	transform.setForward(forward);
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

	if (this->body != nullptr) {
		glm::vec3 pos = this->transform.getPosition();
		rp3d::Vector3 initPosition({ pos.x, pos.y, pos.z });
		rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
		rp3d::Transform transform(initPosition, initOrientation);
		this->body->setTransform(transform);
	}
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

Transform * Entity::getTransform()
{
	return &transform;
}

void Entity::setCollisionBody(rp3d::CollisionBody * body)
{
	this->body = body;
}

rp3d::CollisionBody * Entity::getCollisionBody() const
{
	return this->body;
}
