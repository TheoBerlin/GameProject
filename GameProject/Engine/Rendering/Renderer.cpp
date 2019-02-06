#include "Renderer.h"

#include "../Entity/Entity.h"
#include "../AssetManagement/Mesh.h"
#include "GLAbstraction/Texture.h"

Renderer::Renderer()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	this->testShader = new Shader("./Engine/Rendering/Shaders/EntityShader.vert", "./Engine/Rendering/Shaders/EntityShader.frag");
	this->uniformBuffer = new UniformBuffer(this->testShader->getID(), "Material", 0);
	Material emptyMaterial;
	emptyMaterial.Ka = glm::vec3(0.1f);
	emptyMaterial.Ks = glm::vec3(1.0f);
	this->uniformBuffer->setData((void*)(&emptyMaterial), sizeof(emptyMaterial) - sizeof(emptyMaterial.textures));
}

Renderer::~Renderer()
{
	delete this->testShader;
	delete this->uniformBuffer;
}

void Renderer::setActiveCamera(Camera * camera)
{
	this->activeCamera = camera;
}

void Renderer::push(Entity * entity)
{
	this->renderingList.push_back(entity);
}

void Renderer::drawAll()
{
	for (Entity* entity : this->renderingList)
	{
		draw(entity);
	}
	this->renderingList.clear();
}

void Renderer::draw(Entity * entity)
{
	Model* model = entity->getModel();
	if (model != nullptr)
	{
		Transform& transform = *entity->getTransform();
		this->testShader->bind();
		this->testShader->setUniformMatrix4fv("vp", 1, false, &(this->activeCamera->getVP()[0][0]));
		this->testShader->setUniformMatrix4fv("transform", 1, false, &(transform.getMatrix()[0][0]));
		draw(model);
	}
}

void Renderer::draw(Model * model)
{
	for (size_t i = 0; i < model->meshCount(); i++)
	{
		Mesh* mesh = model->getMesh(i);
		mesh->bindMaterial(this->uniformBuffer);
		
		unsigned int materialIndex = mesh->getMaterialIndex();
		Material& material = model->getMaterial(materialIndex);
		for (Texture* texture : material.textures) {
			this->testShader->setTexture2D("tex", 0, texture->getID());
		}
		
		mesh->bindVertexBuffer();
		IndexBuffer& ib = mesh->getIndexBuffer();
		ib.bind();
		glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0);
	}
}
