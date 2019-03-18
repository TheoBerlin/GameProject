#pragma once

// Enum for the shapes for the CollisionBodies
enum class SHAPE {
	DRONE = 0,
	BOX = 1,
	ARROW = 2,
	SIZE
};

enum CATEGORY
{
	NO_COLLISION = 0x0000,
	ARROW = 0x0001,
	STATIC = 0x0002,
	DRONE_BODY = 0x0004,
	DRONE_EYE = 0x0008
};

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <reactphysics3d/reactphysics3d.h>

struct CollisionShapeDrawingData {
	glm::vec3 color = glm::vec3(1.f);

	glm::vec3 scale = glm::vec3(1.f);
	glm::vec3 size = glm::vec3(1.f);
	glm::vec3 pos = glm::vec3(1.f);
	glm::quat rot = glm::quat(1.f, 1.f, 1.f, 1.f);
	std::string name = "";
	unsigned short category = 0;
	rp3d::CollisionShape * shape = nullptr;
};