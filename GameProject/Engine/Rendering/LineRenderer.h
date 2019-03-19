#pragma once

#include "GLAbstraction/VertexArray.h"
#include "GLAbstraction/IndexBuffer.h"
#include "GLAbstraction/Shader.h"

class LineRenderer
{
public:
	LineRenderer();
	virtual ~LineRenderer();

	void drawLine(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& color = glm::vec3(1.0f));

private:
	VertexArray* va;
	VertexBuffer* vb;
	IndexBuffer* ib;

	Shader* lineShader;
};