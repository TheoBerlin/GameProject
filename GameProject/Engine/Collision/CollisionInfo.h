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

#ifdef ENABLE_COLLISION_BOXES


struct CollisionShapeDrawingData {
	glm::vec3 color;
	glm::vec3 scale;
	glm::vec3 pos;
	unsigned short category;
	rp3d::CollisionShape * shape;
};
#else
struct CollisionShapeDrawingData {
	glm::vec3 pos;
	rp3d::CollisionShape * shape;
	unsigned short category;
};
#endif