#include "EntityManager.h"

#include "Utils/Logger.h"
#include "Entity.h"



EntityManager::EntityManager()
	: reservedEntities(20)
{
	this->entities.reserve(this->reservedEntities);

}


EntityManager::~EntityManager()
{
	this->removeEntities();
}

void EntityManager::update(const float dt)
{
	for (unsigned int i = 0; i < this->entities.size(); i++) {
		this->entities[i]->update(dt);
	}
}

Entity* EntityManager::addEntity()
{
	Entity* newEntity = new Entity();

	this->entities.push_back(newEntity);

	return newEntity;
}

void EntityManager::addEntity(Entity* entity)
{
	this->entities.push_back(entity);
}

void EntityManager::removeEntity(unsigned int index)
{
	if (index < entities.size()) {
		delete entities[index];
		entities.erase(entities.begin() + index);
	}
}

Entity* EntityManager::getEntity(unsigned int index)
{
	Entity* entity = nullptr;
	if ((unsigned)index < entities.size()) {
		entity = entities[index];
	}
	else {
		LOG_ERROR("Entity Manager entity array out of range");
	}
	return entity;
}

int EntityManager::getEntitySize() const
{
	return entities.size();
}

Entity * EntityManager::addTracedEntity(const std::string & name)
{
	auto trace = this->tracedEntities.find(name);
	if (trace == this->tracedEntities.end()) {

		Entity* newEntity = addEntity();
		newEntity->setName(name);

		this->tracedEntities[name] = this->entities.size() - 1;

		return newEntity;
	} 
	else {
		return this->entities[trace->second];
	}


}

// Returns nullptr if entity dosen't exist
Entity * EntityManager::getTracedEntity(const std::string & name)
{
	auto trace = this->tracedEntities.find(name);
	if (trace == this->tracedEntities.end()) {
		return nullptr;
	}
	else {
		return this->entities[trace->second];
	}
}

bool EntityManager::removeTracedEntity(const std::string & name)
{
	auto trace = this->tracedEntities.find(name);

	if (trace != this->tracedEntities.end()) {
		delete this->entities[trace->second];

		if (trace->second != this->entities.size() - 1) {
			this->entities[trace->second] = this->entities.back(); //swap with last
			this->tracedEntities[this->entities[trace->second]->getName()] = trace->second;
			this->tracedEntities.erase(trace);
		}
		else {
			this->tracedEntities.erase(trace);
		}
		this->entities.pop_back();

		return true;
	}

	return false;
}

std::vector<Entity*>& EntityManager::getAll()
{
	return this->entities;
}

void EntityManager::removeEntities()
{
	for (int i = this->entities.size() - 1; i >= 0; i--) {
		delete this->entities[i];
		this->entities[i] = nullptr;
	}
	this->entities.clear();

	this->tracedEntities.clear();
}
