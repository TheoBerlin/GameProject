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

	void print() {
		printf("%d\n", myNum);
	}

private:
	static int num;
	int myNum;

	std::string name;

};

