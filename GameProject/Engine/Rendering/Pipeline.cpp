#include "Pipeline.h"

#include "Engine/AssetManagement/Mesh.h"
#include "Engine/Entity/Entity.h"
#include "Engine/Events/EventBus.h"

Pipeline::Pipeline()
{
	EventBus::get().subscribe(this, &Pipeline::updateFramebufferDimension);

	this->quad = ModelLoader::loadModel("Game/assets/postProcessQuad.fbx");

	/*
		Loading in shaders, Quad and Z pre pass are necessary for drawing texture to window and depth pre pass
		Test shader will be swapped out with a option to choose between multiple shaders for rendering Entities
	*/
	this->quadShader = new Shader("./Engine/Rendering/Shaders/PostProcessVert.vert", "./Engine/Rendering/Shaders/PostProcessFrag.frag");
	this->testShader = new Shader("./Engine/Rendering/Shaders/EntityShader.vert", "./Engine/Rendering/Shaders/EntityShader.frag");
	this->ZprePassShader = new Shader("./Engine/Rendering/Shaders/ZPrepassVert.vert", "./Engine/Rendering/Shaders/ZPrepassFrag.frag");
	this->particleShader = new Shader("./Engine/Particle/Particle.vert", "./Engine/Particle/Particle.frag");

	Display& display = Display::get();
	int width = display.getWidth();
	int height = display.getHeight();
	this->fbo.attachTexture(width, height, AttachmentType::COLOR);
	this->fbo.attachTexture(width, height, AttachmentType::DEPTH);

	/*
		Set up main uniform buffer for material settings
	*/
	this->uniformBuffer = new UniformBuffer();
	this->uniformBuffer->setShader(this->testShader->getID(), "Material", 0);
	Material emptyMaterial;
	emptyMaterial.Ka = glm::vec3(0.1f);
	emptyMaterial.Ks = glm::vec3(1.0f);
	this->uniformBuffer->setData((void*)(&emptyMaterial), sizeof(emptyMaterial) - sizeof(emptyMaterial.textures));

}


Pipeline::~Pipeline()
{
	delete this->quadShader;
	delete this->ZprePassShader;
	delete this->testShader;
	delete this->uniformBuffer;
	delete this->particleShader;
}

Texture* Pipeline::drawParticle(ParticleManager& particleManager)
{
	static const GLfloat g_vertex_buffer_data[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
	};
	this->particleShader->bind();
	if (p) {

		glGenBuffers(1, &billboard_vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

		glGenBuffers(1, &particleDataBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, particleDataBuffer);
		// Initialize with empty (NULL) buffer : it will be updated later, each frame.
		glBufferData(GL_ARRAY_BUFFER, particleManager.getMaxParticles() * sizeof(Particle), NULL, GL_STREAM_DRAW);
	}
	p = false;
	if (particleManager.getParticleCount() != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, particleDataBuffer);
		glBufferData(GL_ARRAY_BUFFER, particleManager.getMaxParticles() * sizeof(Particle), NULL, GL_STREAM_DRAW);
		particleManager.updateBuffer();
	}

	// Quad
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Position + Scale
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, particleDataBuffer);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);

	// Colour
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, particleDataBuffer);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(sizeof(glm::vec3) + sizeof(float)));
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	glVertexAttribDivisor(0, 0); // Reuse quad for every vertex
	glVertexAttribDivisor(1, 1); // Use one Position + scale per quad
	glVertexAttribDivisor(2, 1); // Use one colour per quad


	this->particleShader->setUniformMatrix4fv("vp", 1, false, &(this->camera->getVP()[0][0]));
	this->particleShader->setUniform3f("cameraUp", this->camera->getV()[0][1], this->camera->getV()[1][1], this->camera->getV()[2][1]);
	this->particleShader->setUniform3f("cameraRight", this->camera->getV()[0][0], this->camera->getV()[1][0], this->camera->getV()[2][0]);

	fbo.bind();
	glClear(GL_COLOR_BUFFER_BIT);
	glDepthMask(GL_TRUE);


	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particleManager.getParticleCount());

	fbo.unbind();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	this->particleShader->unbind();
	return fbo.getColorTexture(0);
}

void Pipeline::prePassDepth(const std::vector<Entity*>& renderingList)
{
	this->fbo.bind();
	this->prePassDepthOn();
	this->ZprePassShader->bind();

	//Draw renderingList
	this->ZprePassShader->setUniformMatrix4fv("vp", 1, false, &(this->camera->getVP()[0][0]));
	draw(renderingList);
	
	this->ZprePassShader->unbind();
	this->prePassDepthOff();
	this->fbo.unbind();
}

void Pipeline::prePassDepthOn()
{
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glClear(GL_DEPTH_BUFFER_BIT);

	glDepthFunc(GL_LESS);
	glColorMask(0, 0, 0, 0);

}

void Pipeline::prePassDepthOff()
{
	glDepthFunc(GL_LEQUAL);
	glColorMask(1, 1, 1, 1);
	glDepthMask(GL_FALSE);
}

void Pipeline::drawToScreen(const std::vector<Entity*>& renderingList)
{
	glEnable(GL_DEPTH_TEST);

	//shader->bind();
	this->testShader->bind();

	this->testShader->setUniformMatrix4fv("vp", 1, false, &(this->camera->getVP()[0][0]));
	draw(renderingList, this->testShader);

	this->testShader->unbind();
	//shader->unbind();
}

/*
	Draws to texture and returns it using shader provided
*/
Texture * Pipeline::drawToTexture(const std::vector<Entity*>& renderingList)
{
	glEnable(GL_DEPTH_TEST);

	this->fbo.bind();
	glClear(GL_COLOR_BUFFER_BIT);
	this->testShader->bind();

	this->testShader->setUniformMatrix4fv("vp", 1, false, &(this->camera->getVP()[0][0]));

	draw(renderingList, this->testShader);

	this->testShader->unbind();
	this->fbo.unbind();

	return this->fbo.getColorTexture(0);
}

void Pipeline::drawTextureToQuad(Texture * tex, Texture *text)
{

	glDisable(GL_DEPTH_TEST);

	this->quadShader->bind();
	this->quadShader->setTexture2D("tex", 0, tex->getID());
	this->quadShader->setTexture2D("particles", 1, text->getID());

	Mesh* mesh = this->quad->getMesh(0);
	mesh->bindVertexBuffer();
	IndexBuffer& ib = mesh->getIndexBuffer();
	ib.bind();
	glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0);

	this->quadShader->unbind();
}

void Pipeline::setActiveCamera(Camera * camera)
{
	this->camera = camera;
}

Framebuffer * Pipeline::getFbo()
{
	return &this->fbo;
}

void Pipeline::draw(const std::vector<Entity*>& renderingList)
{
	for (Entity* entity : renderingList)
	{
		Model* model = entity->getModel();
		Transform* transform = entity->getTransform();

		if (model != nullptr)
		{
			this->ZprePassShader->setUniformMatrix4fv("transform", 1, false, &(transform->getMatrix()[0][0]));
			this->drawModelPrePass(model);
		}
	}
}

void Pipeline::draw(const std::vector<Entity*>& renderingList, Shader* shader)
{
	for (Entity* entity : renderingList)
	{
		Model* model = entity->getModel();
		Transform* transform = entity->getTransform();
		
		if (model != nullptr)
		{
			shader->setUniformMatrix4fv("transform", 1, false, &(transform->getMatrix()[0][0]));
			drawModel(model, shader);
		}
	}
}

void Pipeline::drawModelPrePass(Model * model)
{
	for (size_t i = 0; i < model->meshCount(); i++)
	{
		Mesh* mesh = model->getMesh(i);

		mesh->bindVertexBuffer();
		IndexBuffer& ib = mesh->getIndexBuffer();
		ib.bind();
		glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0);
	}
}

void Pipeline::updateFramebufferDimension(WindowResizeEvent * event)
{
	this->fbo.updateDimensions(0, event->width, event->height);
}

void Pipeline::drawModel(Model * model, Shader* shader)
{
	for (size_t i = 0; i < model->meshCount(); i++)
	{
		Mesh* mesh = model->getMesh(i);
		mesh->bindMaterial(this->uniformBuffer);

		unsigned int materialIndex = mesh->getMaterialIndex();
		Material& material = model->getMaterial(materialIndex);

		this->uniformBuffer->setSubData((void*)(&material), sizeof(material) - sizeof(material.textures), 0);

		for (Texture* texture : material.textures) {
			shader->setTexture2D("tex", 0, texture->getID());
		}

		mesh->bindVertexBuffer();
		IndexBuffer& ib = mesh->getIndexBuffer();
		ib.bind();
		glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0);
	}
}
