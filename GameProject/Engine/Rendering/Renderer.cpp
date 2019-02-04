#include "Renderer.h"

#include "../Entity/Entity.h"
#include "../AssetManagement/Mesh.h"

Renderer::Renderer()
{
	this->testShader = new Shader("./Engine/Rendering/Shaders/testShader.vert", "./Engine/Rendering/Shaders/testShader.frag");
	this->uniformBuffer = new UniformBuffer(this->testShader->getID(), "Material", 0);
	Material emptyMaterial;
	emptyMaterial.Ka = glm::vec3(0.1f);
	emptyMaterial.Ks = glm::vec3(1.0f);
	this->uniformBuffer->setData((void*)(&emptyMaterial), sizeof(emptyMaterial) - sizeof(emptyMaterial.Textures));
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

void Renderer::draw(Entity * entity)
{
	Model* model = entity->getModel();
	if (model != nullptr)
	{
		this->testShader->bind();
		this->testShader->setUniformMatrix4fv("vp", 1, false, &(this->activeCamera->getVP()[0][0]));
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
		for (Texture& texture : material.Textures) {
			this->testShader->setTexture2D("tex", 0, texture.id);
		}
		
		mesh->bindVertexBuffer();
		IndexBuffer& ib = mesh->getIndexBuffer();
		ib.bind();
		glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0);
	}
}
