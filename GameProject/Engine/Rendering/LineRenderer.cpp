#include "LineRenderer.h"

#include "Engine/AssetManagement/Mesh.h"
#include "Display.h"
#include "Renderer.h"

LineRenderer::LineRenderer()
{
	this->lineShader = new Shader("./Engine/Rendering/Shaders/DebugShaders/LineShader.vert", "./Engine/Rendering/Shaders/DebugShaders/LineShader.frag");
	this->va = nullptr;
	this->vb = nullptr;
	this->ib = nullptr;
}

LineRenderer::~LineRenderer()
{
	delete this->lineShader;
	delete this->va; // va will delete ib.
	delete this->ib;
}

void LineRenderer::drawLine(const glm::vec3 & v1, const glm::vec3 & v2, const glm::vec3 & color)
{
	float vertices[] = {
		v1.x, v1.y, v1.z,
		v2.x, v2.y, v2.z
	};

	unsigned indices[] = { 0, 1 };

	delete va;
	this->va = new VertexArray();
	AttributeLayout layout;
	layout.push(3); // vec3 position
	this->vb = new VertexBuffer(vertices, sizeof(vertices), GL_STATIC_DRAW);
	this->va->addBuffer(this->vb, layout);

	delete this->ib;
	this->va->bind();
	this->ib = new IndexBuffer(indices, 2, GL_STATIC_DRAW);

	this->lineShader->bind();
	this->lineShader->setUniform3fv("color", 1, &color[0]);
	this->lineShader->setUniformMatrix4fv("vp", 1, false, &(Display::get().getRenderer().getActiveCamera()->getVP()[0][0]));
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
}
