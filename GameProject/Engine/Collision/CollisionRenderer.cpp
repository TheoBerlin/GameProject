#include "CollisionRenderer.h"

#include "Engine/AssetManagement/Mesh.h"

#include "Engine/AssetManagement/ModelLoader.h"
#include "Engine/Rendering/Display.h"
#include "Engine/Rendering/Renderer.h"

CollisionRenderer::CollisionRenderer()
{
	this->shader = new Shader("./Engine/Rendering/Shaders/CollisionBox.vert", "./Engine/Rendering/Shaders/CollisionBox.frag");

	float cubeVertices[] = {
		// front
		-1.0, -1.0,  1.0,
		1.0, -1.0,  1.0,
		1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
		// back
		-1.0, -1.0, -1.0,
		1.0, -1.0, -1.0,
		1.0,  1.0, -1.0,
		-1.0,  1.0, -1.0
	};

	unsigned indices[] = {
		0, 1, 2, 3, 0,
		4, 7, 3, 2, 6,
		7, 4, 5, 6, 2,
		1, 5
	};
	AttributeLayout layout;
	layout.push(3); // vec3 position

	//Load collision Box model
	this->collisionBoxMesh = new Mesh((void*)cubeVertices, sizeof(float) * 24, (void*)indices, 17, layout);

	AttributeLayout colorLayout;
	colorLayout.push(3, 1); // Vec3 Color 

	this->collisionBoxMesh->addBuffer(NULL, 0, colorLayout);

	AttributeLayout matrixLayout;
	matrixLayout.push(4, 1); // Vec4 Row 1 
	matrixLayout.push(4, 1); // Vec4 Row 2
	matrixLayout.push(4, 1); // Vec4 Row 3
	matrixLayout.push(4, 1); // Vec4 Row 4

	this->collisionBoxMesh->addBuffer(NULL, 0, matrixLayout);

	this->instanceCount = 0;
}


CollisionRenderer::~CollisionRenderer()
{
	delete this->collisionBoxMesh;
	delete this->shader;
}


void CollisionRenderer::updateMatrices(const std::vector<glm::mat4>& matrices)
{
	this->collisionBoxMesh->updateInstancingData((void*)&matrices[0][0][0], matrices.size() * sizeof(glm::mat4), 0, 2);
	this->instanceCount = matrices.size();
}

void CollisionRenderer::updateColors(const std::vector<glm::vec3>& colors)
{
	this->collisionBoxMesh->updateInstancingData((void*)&colors[0][0], colors.size() * sizeof(glm::vec3), 0);
}

void CollisionRenderer::render()
{
	this->shader->bind();
	this->collisionBoxMesh->bindVertexArray();

	this->shader->setUniformMatrix4fv("vp", 1, false, &(Display::get().getRenderer().getActiveCamera()->getVP()[0][0]));


	IndexBuffer& ib = this->collisionBoxMesh->getIndexBuffer();
	ib.bind();
	glLineWidth(2);
	glDrawElementsInstanced(GL_LINE_STRIP, ib.getCount(), GL_UNSIGNED_INT, 0, this->instanceCount);

}
