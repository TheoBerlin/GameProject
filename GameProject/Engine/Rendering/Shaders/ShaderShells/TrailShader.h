#pragma once

#include <string>

#include "Engine/Rendering/GLAbstraction/Shader.h"
#include "Engine/AssetManagement/Mesh.h"
class Camera;

class TrailShader : public Shader
{
public:
	TrailShader(const std::string & vertex, const std::string & fragment, Camera ** camera);
	~TrailShader();

	void bind();
	void updateMeshData(unsigned texId);

	virtual void update(const float& dt);
private:
	VertexArray vao;

	Mesh * mesh;
	Camera** camera;
};

