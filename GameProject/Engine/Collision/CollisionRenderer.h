#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "Engine/Rendering/GLAbstraction/Shader.h"

class Entity;
class Mesh;

class CollisionRenderer
{
public:
	CollisionRenderer();
	~CollisionRenderer();

	void updateMatrices(const std::vector<glm::mat4>& matrices);
	void updateColors(const std::vector<glm::vec3>& colors);
	void render();

private:
	Shader* shader;

	// Used for rendering collision boxes
	Mesh* collisionBoxMesh;
	unsigned instanceCount;
};

