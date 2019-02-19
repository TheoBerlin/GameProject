#pragma once
#include "Engine/Rendering/GLAbstraction/Shader.h"
#include "Engine/Rendering/GLAbstraction/VertexArray.h"

#include <string>

class ParticleRendering {
private:
	Shader* particleShader;
	VertexArray vao;
	AttributeLayout layout;

public:
	ParticleRendering();

	void init(std::string vertexShader, std::string fragmentShader);

	void bind();
	void unbind();
};