#pragma once

#include <vector>
#include <unordered_map>

#include "../../Utils/Logger.h"
#include "Entity.h"

class EntityManager
{
public:
	EntityManager();
	~EntityManager();

	void update(const float dt);

	Entity* addEntity();  //Used for entites that are static wont change until level is unloaded.
	void addEntity(Entity* entity);
	Entity* getEntity(unsigned int index);
	int getEntitySize() const;
	Entity* addTracedEntity(const std::string & name); //Used for entites that we want to track, and do changes to during runtime.
	Entity* getTracedEntity(const std::string & name);
	bool removeTracedEntity(const std::string & name);

	void removeEntities();

private:
	std::vector<Entity*> entities;
	std::unordered_map<std::string, unsigned> tracedEntities; // unsigned to keep track of position in vector

	
	const unsigned reservedEntities; // Estimated avarege of entities

};

