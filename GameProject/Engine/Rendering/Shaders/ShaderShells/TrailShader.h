#pragma once

#include <string>

#include "Engine/Rendering/GLAbstraction/Shader.h"
#include "Engine/AssetManagement/Mesh.h"

class Camera;
struct TrailPointData;

class TrailShader : public Shader
{
public:
	TrailShader(Camera ** camera);
	~TrailShader();

	void bind();
	void unbind();

	void setHorizontal(bool horiz = true);

	size_t getDrawCount() const;

	void updateTrail(const std::vector<TrailPointData>& pointData, const glm::vec3& color = glm::vec3(1.0f, 0.0f, 0.0f));
private:
	VertexArray vao;

	glm::vec3 trailColor;

	bool horizontal;
	size_t pointCount;

	Mesh * mesh;
	Camera** camera;
};

