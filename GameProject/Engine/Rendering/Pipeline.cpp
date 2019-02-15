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
	this->entityShaderInstanced = new Shader("./Engine/Rendering/Shaders/EntityShaderInstanced.vert", "./Engine/Rendering/Shaders/EntityShaderInstanced.frag");
	this->quadShader = new Shader("./Engine/Rendering/Shaders/PostProcessVert.vert", "./Engine/Rendering/Shaders/PostProcessFrag.frag");
	this->testShader = new Shader("./Engine/Rendering/Shaders/EntityShader.vert", "./Engine/Rendering/Shaders/EntityShader.frag");
	this->ZprePassShader = new Shader("./Engine/Rendering/Shaders/ZPrepassVert.vert", "./Engine/Rendering/Shaders/ZPrepassFrag.frag");
	this->particleShader = new Shader("./Engine/Particle/Particle.vert", "./Engine/Particle/Particle.frag");

	Display& display = Display::get();

	int width = display.getWidth();
	int height = display.getHeight();
	this->fbo.attachTexture(width, height, AttachmentType::COLOR);
	this->fbo.attachTexture(width, height, AttachmentType::DEPTH);


	this->uniformBuffers.resize(7);
	for (UniformBuffer* ubo : this->uniformBuffers)
		ubo = nullptr;

	/*
		Set up uniform buffers for shaders
	*/
	this->addUniformBuffer(0, this->testShader->getID(), "Material");
	this->addUniformBuffer(1, this->testShader->getID(), "DirectionalLight");

	this->addUniformBuffer(0, this->entityShaderInstanced->getID(), "Material");
	this->addUniformBuffer(1, this->entityShaderInstanced->getID(), "DirectionalLight");

	/*
		Set up Directional Light
	*/
	this->mainLight.direction = glm::normalize(glm::vec4(0.5f, -1.0f, -0.5f, 1.0f));
	this->mainLight.color_intensity = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->uniformBuffers[1]->setSubData((void*)&this->mainLight, sizeof(this->mainLight), 0);
}


Pipeline::~Pipeline()
{
	delete this->quadShader;
	delete this->ZprePassShader;
	delete this->testShader;
	delete this->particleShader;
	delete this->entityShaderInstanced;

	for (UniformBuffer* ubo : this->uniformBuffers)
		delete ubo;

	this->uniformBuffers.clear();
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
	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Position + Scale
	glEnableVertexAttribArray(5);
	glBindBuffer(GL_ARRAY_BUFFER, particleDataBuffer);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);

	// Colour
	glEnableVertexAttribArray(6);
	glBindBuffer(GL_ARRAY_BUFFER, particleDataBuffer);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(sizeof(glm::vec3) + sizeof(float)));
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glVertexAttribDivisor(4, 0); // Reuse quad for every vertex
	glVertexAttribDivisor(5, 1); // Use one Position + scale per quad
	glVertexAttribDivisor(6, 1); // Use one colour per quad


	this->particleShader->setUniformMatrix4fv("vp", 1, false, &(this->camera->getVP()[0][0]));
	this->particleShader->setUniform3f("cameraUp", this->camera->getView()[0][1], this->camera->getView()[1][1], this->camera->getView()[2][1]);
	this->particleShader->setUniform3f("cameraRight", this->camera->getView()[0][0], this->camera->getView()[1][0], this->camera->getView()[2][0]);

	fbo.bind();
	glClear(GL_COLOR_BUFFER_BIT);
	glDepthMask(GL_TRUE);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particleManager.getParticleCount());

	fbo.unbind();

	glDisableVertexAttribArray(4);
	glDisableVertexAttribArray(5);
	glDisableVertexAttribArray(6);

	this->particleShader->unbind();
	
	return fbo.getColorTexture(0);
}

void Pipeline::prePassDepth(const std::vector<Entity*>& renderingList, bool toScreen)
{
	if(!toScreen)
		this->fbo.bind();
	this->prePassDepthOn();
	this->ZprePassShader->bind();

	//Draw renderingList
	this->ZprePassShader->setUniformMatrix4fv("vp", 1, false, &(this->camera->getVP()[0][0]));
	draw(renderingList);
	
	this->ZprePassShader->unbind();
	this->prePassDepthOff();
	if (!toScreen)
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

void Pipeline::addUniformBuffer(unsigned bindingPoint, const unsigned shaderID, const char* blockName)
{
	UniformBuffer * ubo = nullptr;
	try {
		ubo = this->uniformBuffers.at(bindingPoint);
	}
	catch (const std::out_of_range& e) {
		(void)e;
		LOG_ERROR("Out of Range error, no Uniform buffer can exist at that bindingpoint, 7 bindingpoints allowed.");
		return;
	}

	if (this->uniformBuffers.at(bindingPoint) == nullptr) {
		ubo = new UniformBuffer();

		ubo->bindShader(shaderID, blockName, bindingPoint);
		ubo->bind(bindingPoint);

		this->uniformBuffers[bindingPoint] = ubo;
	}
	else {
		ubo->bindShader(shaderID, blockName, bindingPoint);
		return;
	}
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
	this->testShader->setUniform3fv("camPos", 1, &this->camera->getPosition()[0]);

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
	mesh->bindVertexArray();
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

		mesh->bindVertexArray();
		IndexBuffer& ib = mesh->getIndexBuffer();
		ib.bind();
		glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0);
	}
}

void Pipeline::drawInstanced(Model * model)
{

	this->entityShaderInstanced->bind();

	this->entityShaderInstanced->setUniformMatrix4fv("vp", 1, false, &(this->camera->getVP()[0][0]));
	this->entityShaderInstanced->setUniform3fv("camPos", 1, &this->camera->getPosition()[0]);

	for (size_t i = 0; i < model->meshCount(); i++)
	{
		Mesh* mesh = model->getMesh(i);

		unsigned int materialIndex = mesh->getMaterialIndex();
		Material& material = model->getMaterial(materialIndex);

		this->uniformBuffers[0]->setSubData((void*)&material, sizeof(material) - sizeof(material.textures), 0);

		for (Texture* texture : material.textures) {
			this->entityShaderInstanced->setTexture2D("tex", 0, texture->getID());
		}

		mesh->bindVertexArray();

		IndexBuffer& ib = mesh->getIndexBuffer();
		ib.bind();
		glDrawElementsInstanced(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0, model->getRenderingGroup().size());
	}
	this->entityShaderInstanced->unbind();
}

void Pipeline::updateFramebufferDimension(WindowResizeEvent * event)
{
	this->fbo.updateDimensions(0, event->width, event->height);
}

void Pipeline::drawModel(Model * model, Shader* shader)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (size_t i = 0; i < model->meshCount(); i++)
	{
		Mesh* mesh = model->getMesh(i);

		unsigned int materialIndex = mesh->getMaterialIndex();
		Material& material = model->getMaterial(materialIndex);

		this->uniformBuffers[0]->setSubData((void*)&material, sizeof(material) - sizeof(material.textures), 0);

		for (Texture* texture : material.textures) {
			shader->setTexture2D("tex", 0, texture->getID());
		}

		mesh->bindVertexArray();
		IndexBuffer& ib = mesh->getIndexBuffer();
		ib.bind();
		glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0);
	}
}
