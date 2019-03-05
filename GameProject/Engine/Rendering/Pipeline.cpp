#include "Pipeline.h"

#include "Engine/AssetManagement/Mesh.h"
#include "Engine/Entity/Entity.h"
#include "Engine/Events/EventBus.h"
#include "Utils/Logger.h"

#include "../Config.h"
#include <Engine/Imgui/imgui.h>

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
	this->ZprePassShaderInstanced = new Shader("./Engine/Rendering/Shaders/ZPrepassInstanced.vert", "./Engine/Rendering/Shaders/ZPrepassInstanced.frag");
	Display& display = Display::get();

	int width = display.getWidth();
	int height = display.getHeight();
	this->fbo.attachTexture(width, height, AttachmentType::COLOR);
	this->fbo.attachTexture(width, height, AttachmentType::DEPTH);

	/*float shadowResScale = 2.0f;
	shadowWidth = Display::get().getWidth() * shadowResScale;
	shadowHeight = Display::get().getHeight() * shadowResScale;*/
	this->shadowFbo.attachTexture((int)lm.getShadowWidthScaled(), (int)lm.getShadowHeightScaled(), AttachmentType::DEPTH);


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
	this->addUniformBuffer(2, this->entityShaderInstanced->getID(), "LightBuffer");

	/*
		Set up Directional Light
	*/
	lm.createDirectionalLight(glm::vec4(0.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	this->uniformBuffers[1]->setSubData((void*)lm.getDirectionalLight(), 32, 0); //no idea how to solve the size issue

	lm.createPointLight(glm::vec4(0.0f, 1.0f, -10.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 7);
	lm.createPointLight(glm::vec4(0.0f, 1.0f, 10.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 7);
	lm.createPointLight(glm::vec4(5.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 7);
	lm.createPointLight(glm::vec4(-5.0f, 1.0f, 0.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), 7);

	struct LightBuffer {
		PointLight pointLights[25];
		int nrOfPointLights;
		glm::vec3 padding;
	} lightBuffer;

	lightBuffer.nrOfPointLights = lm.getNrOfPointLights();

	for (int i = 0; i < lm.getNrOfPointLights(); i++) {
		lightBuffer.pointLights[i] = *lm.getPointLights()->at(i);
	}

	this->uniformBuffers[2]->setSubData((void*)(&lightBuffer), sizeof(lightBuffer), 0);

	//this->uniformBuffers[2]->setSubData((void*)(&lightBuffer.nrOfPointLights), 4, 0);
	//this->uniformBuffers[2]->setSubData((void*)(&lightBuffer.pointLights[0]), sizeof(lightBuffer)-4, 4);
}


Pipeline::~Pipeline()
{
	delete this->quadShader;
	delete this->ZprePassShader;
	delete this->ZprePassShaderInstanced;
	delete this->testShader;
	delete this->entityShaderInstanced;

	for(UniformBuffer* ubo : this->uniformBuffers)
		delete ubo;

	this->uniformBuffers.clear();
}

void Pipeline::prePassDepth(const std::vector<Entity*>& renderingList, bool toScreen)
{
	if(!toScreen)
		this->fbo.bind();;
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

void Pipeline::prePassDepthModel(const std::vector<Model*>& renderingModels, bool toScreen)
{
	if (!toScreen)
		this->fbo.bind();
	this->prePassDepthOn();
	this->ZprePassShaderInstanced->bind();

	//Draw renderingList
	this->ZprePassShaderInstanced->setUniformMatrix4fv("vp", 1, false, &(this->camera->getVP()[0][0]));
	
	for (Model* model : renderingModels) {
		drawModelPrePassInstanced(model);
	}

	this->ZprePassShaderInstanced->unbind();
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

	this->testShader->bind();

	this->testShader->setUniformMatrix4fv("vp", 1, false, &(this->camera->getVP()[0][0]));
	draw(renderingList, this->testShader);

	this->testShader->unbind();
}

void Pipeline::drawModelToScreen(const std::vector<Model*>& renderingModels)
{
	glEnable(GL_DEPTH_TEST);

	this->entityShaderInstanced->bind();

	this->entityShaderInstanced->setUniformMatrix4fv("vp", 1, false, &(this->camera->getVP()[0][0]));
	this->entityShaderInstanced->setUniform3fv("camPos", 1, &this->camera->getPosition()[0]);

	for (Model* model : renderingModels) {
		drawInstanced(model);
	}

	this->entityShaderInstanced->unbind();
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
	this->testShader->setUniformMatrix4fv("lightMatrix", 1, false, &(lm.getLightMatrix()[0][0]));
	this->testShader->setUniform3fv("camPos", 1, &this->camera->getPosition()[0]);

	Texture * shadowTex = getShadowFbo()->getDepthTexture();
	this->testShader->setTexture2D("shadowTex", 1, shadowTex->getID());

	draw(renderingList, this->testShader);

	this->testShader->unbind();
	this->fbo.unbind();

	return this->fbo.getColorTexture(0);
}

Texture * Pipeline::drawModelToTexture(const std::vector<Model*>& renderingModels)
{

	this->fbo.bind();
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);

	this->entityShaderInstanced->bind();

	this->entityShaderInstanced->setUniformMatrix4fv("lightMatrix", 1, false, &(lm.getLightMatrix()[0][0]));
	this->entityShaderInstanced->setUniformMatrix4fv("vp", 1, false, &(this->camera->getVP()[0][0]));
	this->entityShaderInstanced->setUniform3fv("camPos", 1, &this->camera->getPosition()[0]);

	Texture * shadowTex = getShadowFbo()->getDepthTexture();
	this->entityShaderInstanced->setTexture2D("shadowTex", 1, shadowTex->getID());

	for (Model* model : renderingModels) {
		drawInstanced(model);
	}

	this->entityShaderInstanced->unbind();
	this->fbo.unbind();

	return this->fbo.getColorTexture(0);
}

void Pipeline::drawTextureToQuad(Texture * tex)
{

	glDisable(GL_DEPTH_TEST);

	this->quadShader->bind();
	this->quadShader->setTexture2D("tex", 0, tex->getID());

	Mesh* mesh = this->quad->getMesh(0);
	mesh->bindVertexArray();
	IndexBuffer& ib = mesh->getIndexBuffer();
	ib.bind();
	glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0);

	this->quadShader->unbind();
}

void Pipeline::calcDirLightDepth(const std::vector<Entity*>& renderingList/*, const glm::vec3 & lightDir*/)
{
	
	int displayWidth = Display::get().getWidth();
	int displayHeight = Display::get().getHeight();

	Display::get().updateView(int(lm.getShadowWidthScaled()), int(lm.getShadowHeightScaled()));

	this->shadowFbo.bind();;
	this->prePassDepthOn();
	this->ZprePassShader->bind();

	/*float orthoWidth = 20.0f;
	float orthoHeight = 20.0f * Display::get().getRatio();
	glm::mat4 lightProjection = glm::ortho(-((float)orthoWidth /2.0f), ((float)orthoWidth / 2.0f), -((float)orthoHeight / 2.0f), ((float)orthoHeight / 2.0f), 0.1f, 100.0f);
	glm::mat4 lightView = glm::lookAt(glm::vec3(0.0f, 20.0f, 10.0f), glm::vec3(mainLight.direction), glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;
*/
	//Draw renderingList
	this->ZprePassShader->setUniformMatrix4fv("vp", 1, false, &lm.getLightMatrix()[0][0]);
	glCullFace(GL_FRONT);
	draw(renderingList);
	glCullFace(GL_BACK);

	this->ZprePassShader->unbind();
	this->prePassDepthOff();
	this->shadowFbo.unbind();

#ifdef IMGUI
	auto drawTexture = [](Texture* texture, bool nextLine = false) {
		ImTextureID texID = (ImTextureID)texture->getID();
		float ratio = (float)texture->getWidth() / (float)texture->getHeight();
		ImGui::Image(texID, ImVec2(50 * ratio, 50), ImVec2(0, 1), ImVec2(1, 0));
		if (nextLine)
			ImGui::SameLine();
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Image(texID, ImVec2(370 * ratio, 370), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
			ImGui::EndTooltip();
		}
	};

	ImGui::Begin("Shadow buffer");

	drawTexture(this->shadowFbo.getDepthTexture());

	ImGui::End();
#endif

	Display::get().updateView(displayWidth, displayHeight);
}

void Pipeline::calcDirLightDepthInstanced(const std::vector<Model*>& renderingModels)
{
	int displayWidth = Display::get().getWidth();
	int displayHeight = Display::get().getHeight();

	Display::get().updateView((int)lm.getShadowWidthScaled(), (int)lm.getShadowHeightScaled());

	this->shadowFbo.bind();
	this->prePassDepthOn();
	this->ZprePassShaderInstanced->bind();

	//Draw renderingList
	this->ZprePassShaderInstanced->setUniformMatrix4fv("vp", 1, false, &lm.getLightMatrix()[0][0]);
	for (Model* model : renderingModels) {
		drawModelPrePassInstanced(model);
	}

	this->ZprePassShaderInstanced->unbind();
	this->prePassDepthOff();
	this->shadowFbo.unbind();

	Display::get().updateView(displayWidth, displayHeight);
}

void Pipeline::setActiveCamera(Camera * camera)
{
	this->camera = camera;
}

Camera * Pipeline::getActiveCamera()
{
	return this->camera;
}

Framebuffer * Pipeline::getFbo()
{
	return &this->fbo;
}

Framebuffer * Pipeline::getShadowFbo()
{
	return &this->shadowFbo;
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

void Pipeline::drawModelPrePassInstanced(Model * model)
{
	for (size_t i = 0; i < model->meshCount(); i++)
	{
		Mesh* mesh = model->getMesh(i);

		mesh->bindVertexArray();

		IndexBuffer& ib = mesh->getIndexBuffer();
		ib.bind();
		glDrawElementsInstanced(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0, model->getRenderingGroup().size());
	}
}
