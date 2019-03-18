#include "CollisionRenderer.h"

#include "Engine/AssetManagement/Mesh.h"

#include "Engine/AssetManagement/ModelLoader.h"
#include "Engine/Rendering/Display.h"
#include "Engine/Rendering/Renderer.h"

#include "CollisionConfig.h"

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
	this->collisionBoxMesh = new Mesh((void*)cubeVertices, sizeof(cubeVertices), (void*)indices, sizeof(indices) / sizeof(unsigned), layout);

	AttributeLayout colorLayout;
	colorLayout.push(3, 1); // Vec3 Color 

	this->collisionBoxMesh->addBuffer(NULL, 0, colorLayout);

	AttributeLayout matrixLayout;
	matrixLayout.push(4, 1); // Vec4 Row 1 
	matrixLayout.push(4, 1); // Vec4 Row 2
	matrixLayout.push(4, 1); // Vec4 Row 3
	matrixLayout.push(4, 1); // Vec4 Row 4

	this->collisionBoxMesh->addBuffer(NULL, 0, matrixLayout);

	this->instanceCountBox = 0;
	this->instanceCountLine = 0;

	// Line mesh
	{
		float cubeVertices[] = {
			-1.0f, -1.0f, -1.0f,
			1.0f, 1.0f, 1.0f
		};

		unsigned indices[] = {
			0, 1
		};
		
		AttributeLayout layout;
		layout.push(3); // vec3 position

		//Load collision Box model
		this->lineMesh = new Mesh((void*)cubeVertices, sizeof(cubeVertices), (void*)indices, sizeof(indices) / sizeof(unsigned), layout, GL_DYNAMIC_DRAW);

		AttributeLayout colorLayout;
		colorLayout.push(3); // Vec3 Color 

		this->lineMesh->addBuffer(NULL, 0, colorLayout);

		AttributeLayout matrixLayout;
		matrixLayout.push(4, 1); // Vec4 Row 1 
		matrixLayout.push(4, 1); // Vec4 Row 2
		matrixLayout.push(4, 1); // Vec4 Row 3
		matrixLayout.push(4, 1); // Vec4 Row 4

		this->lineMesh->addBuffer(NULL, 0, matrixLayout);
	}
}

CollisionRenderer::~CollisionRenderer()
{
	delete this->lineMesh;
	delete this->collisionBoxMesh;
	delete this->shader;
}

void CollisionRenderer::updateLines(const std::vector<glm::vec3>& lines)
{
	this->lineMesh->bindVertexArray();
	this->lineMesh->updateInstancingData((void*)&(lines[0][0]), lines.size() * sizeof(glm::vec3), 0, 0);
	IndexBuffer& ibLine = this->lineMesh->getIndexBuffer();
	std::vector<unsigned> indices;
	for (unsigned int i = 0; i < lines.size(); i++)
		indices.push_back(i);
	ibLine.updateData((void*)&indices[0], indices.size());
	this->instanceCountLine = lines.size()/2;
}

void CollisionRenderer::updateMatricesLine(const std::vector<glm::mat4>& matrices)
{
	this->lineMesh->bindVertexArray();
	this->lineMesh->updateInstancingData((void*)&matrices[0][0][0], matrices.size() * sizeof(glm::mat4), 0, 2);
	this->instanceCountLine = matrices.size();
}

void CollisionRenderer::updateColorsLine(const std::vector<glm::vec3>& colors)
{
	this->lineMesh->updateInstancingData((void*)&colors[0][0], colors.size() * sizeof(glm::vec3), 0, 1);
}

void CollisionRenderer::updateMatrices(const std::vector<glm::mat4>& matrices)
{
	this->collisionBoxMesh->updateInstancingData((void*)&matrices[0][0][0], matrices.size() * sizeof(glm::mat4), 0, 2);
	this->instanceCountBox = matrices.size();
}

void CollisionRenderer::updateColors(const std::vector<glm::vec3>& colors)
{
	this->collisionBoxMesh->updateInstancingData((void*)&colors[0][0], colors.size() * sizeof(glm::vec3), 0, 1);
}

void CollisionRenderer::render()
{
	this->shader->bind();
	this->shader->setUniformMatrix4fv("vp", 1, false, &(Display::get().getRenderer().getActiveCamera()->getVP()[0][0]));

#ifdef ENABLE_COLLISION_BOXES
	// Draw boxes.
	this->collisionBoxMesh->bindVertexArray();
	IndexBuffer& ibBox = this->collisionBoxMesh->getIndexBuffer();
	ibBox.bind();
	glDrawElementsInstanced(GL_LINE_STRIP, ibBox.getCount(), GL_UNSIGNED_INT, 0, this->instanceCountBox);
#endif

	// Draw lines.
#ifdef ENABLE_AXIS_FOR_COLLISION_BOXES
	this->lineMesh->bindVertexArray();
	IndexBuffer& ibLine = this->lineMesh->getIndexBuffer();
	ibLine.bind();
	glDrawElementsInstanced(GL_LINES, ibLine.getCount(), GL_UNSIGNED_INT, 0, this->instanceCountLine);
#endif
}
