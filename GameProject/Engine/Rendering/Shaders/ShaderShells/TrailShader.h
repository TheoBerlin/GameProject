#pragma once

#include <string>

#include "Engine/Rendering/GLAbstraction/Shader.h"
#include "Engine/AssetManagement/Mesh.h"

class Camera;

class TrailShader : public Shader
{
public:
	TrailShader(Camera ** camera);
	~TrailShader();

	void bind();
	void unbind();

	void setHorizontal(bool horiz = true);

	size_t getDrawCount() const;

	void updateTrail(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& upVectors);
private:
	VertexArray vao;

	bool horizontal;
	size_t pointCount;

	Mesh * mesh;
	Camera** camera;
};

