#include "Entity.h"

#include "reactphysics3d/reactphysics3d.h"

Entity::Entity(const glm::vec3& forward) : model(nullptr)
{
	transform.setForward(forward);

	this->renderingGroupIndex = -1;
	this->hasSeparatedTransforms = false;
	this->model = nullptr;
}

Entity::~Entity()
{
	removeAllComponents();

	/*
		Remove entity from models rendering group
	*/
	if (this->model != nullptr && this->renderingGroupIndex != -1) {
		this->model->removeEntity(this->renderingGroupIndex);
	}
}

void Entity::update(const float dt)
{
	for (auto& it : this->components)
		it.second->update(dt);

	// Save the current transform if not separated.
	if (!this->hasSeparatedTransforms)
		this->pausedTransform = this->transform;

	/*
		Updates vertex buffer of model, if it exists and a component has moved the entity
	*/
	if (this->model && this->getTransform()->getStatus() && this->renderingGroupIndex != -1) {
		this->model->updateInstancingSpecificData(this->renderingGroupIndex);

		if (this->body != nullptr) {
			glm::vec3 pos = this->transform.getPosition();
			rp3d::Vector3 newPos({ pos.x, pos.y, pos.z });
			rp3d::Quaternion newRot;
			glm::quat q = this->transform.getRotationQuat();
			newRot.setAllValues(q.x, q.y, q.z, q.w);

			rp3d::Transform transform(newPos, newRot);
			this->body->setTransform(transform);
		}
	}
}

bool Entity::addComponent(Component * component)
{
	auto ite = this->components.find(component->getName());

	if (ite == this->components.end()) {
		this->components[component->getName()] = component;
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

void Entity::removeAllComponents()
{
	for (auto& it : this->components) {
		delete it.second;
		it.second = nullptr;
	}

	this->components.clear();
}

bool Entity::detachComponent(const std::string& componentName)
{
	auto ite = this->components.find(componentName);

	if (ite != this->components.end()) {
		this->components.erase(ite);
		return true;
	}

	return false;
}

Component* Entity::getComponent(const std::string& componentName)
{
	// Returns nullptr if component is not found
	auto elem = this->components.find(componentName);

	if (elem != this->components.end()) {
		return elem->second;
	} else {
		return nullptr;
	}
}

void Entity::setModel(Model * model)
{
	if (model != nullptr) {
		/*
			Remove itself from old model, Complexity: Linear on the entities after the entity deleted
		*/
		if (this->renderingGroupIndex != -1)
			this->model->removeEntity(this->renderingGroupIndex);

		this->renderingGroupIndex = model->addEntity(this);
		this->model = model;
		this->model->updateInstancingData();
	}
	else {
		this->model = nullptr;
	}
}

void Entity::detachFromModel()
{
	if (this->model != nullptr && this->renderingGroupIndex != -1)
		this->model->removeEntity(this->renderingGroupIndex);

	this->renderingGroupIndex = -1;
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

void Entity::attachToModel()
{
	if (this->model != nullptr && renderingGroupIndex == -1)
		this->renderingGroupIndex = this->model->addEntity(this);
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
	return &this->transform;
}

Transform * Entity::getPausedTransform()
{
	return &this->pausedTransform;
}

void Entity::pauseModelTransform()
{
	this->hasSeparatedTransforms = true;
	this->pausedTransform = this->transform;
}

void Entity::unpauseModelTransform()
{
	this->hasSeparatedTransforms = false;
}

bool Entity::isTransformSeparated() const
{
	return this->hasSeparatedTransforms;
}

void Entity::setCollisionBody(rp3d::CollisionBody * body)
{
	this->body = body;
}

rp3d::CollisionBody * Entity::getCollisionBody() const
{
	return this->body;
}
