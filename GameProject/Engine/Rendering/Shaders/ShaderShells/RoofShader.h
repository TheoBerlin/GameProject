#pragma once

#include "Engine/Rendering/Shaders/ShaderShells/EntityShader.h"
#include "Engine/Rendering/GLAbstraction/Texture.h"

class RoofShader : public EntityShader
{
public:
	RoofShader(Camera ** camera);
	virtual ~RoofShader();

	void bind();
	void updateMeshData(unsigned texId);
	void update(const float& dt);

private:
	Texture* wallTexture;
};

