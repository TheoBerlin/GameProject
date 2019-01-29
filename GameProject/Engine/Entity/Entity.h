#pragma once

#include <string>

class Entity
{
public:
	Entity();
	~Entity();

	void update(const float dt);

	void setName(const std::string& name);
	const std::string getName();

private:
	std::string name;

};

