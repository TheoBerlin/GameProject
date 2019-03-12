#include "Pipeline.h"

#include "Engine/AssetManagement/Mesh.h"
#include "Engine/Entity/Entity.h"
#include "Engine/Events/EventBus.h"
#include "Utils/Logger.h"
#include "Display.h"
#include "../Config.h"
#include <Engine/Imgui/imgui.h>
#include "Engine/Rendering/Shaders/ShaderShells/DroneShader.h"
#include "Engine/Rendering/Shaders/ShaderShells/WallShader.h"
#include "Engine/Rendering/Shaders/ShaderShells/InfinityPlaneShader.h"
#include "Engine/Rendering/Shaders/ShaderShells/InfinityPlanePrePassShader.h"

#include "Engine/Rendering/Shaders/ShaderShells/PostProcess/QuadShader.h"
#include "Engine/Rendering/Shaders/ShaderShells/PostProcess/BlurShader.h"



Pipeline::Pipeline()
{
	EventBus::get().subscribe(this, &Pipeline::updateFramebufferDimension);
	
	glm::mat4 * identityMatrix = &glm::mat4(1.0f);

	//Create quad for drwaing textures to.
	this->createQuad();

	/*
		Loading in shaders, Quad and Z pre pass are necessary for drawing texture to window and depth pre pass
		Test shader will be swapped out with a option to choose between multiple shaders for rendering Entities
	*/
	this->entityShaders.push_back(new EntityShader("./Engine/Rendering/Shaders/EntityShaderInstanced.vert", "./Engine/Rendering/Shaders/EntityShaderInstanced.frag",
											&this->shadowFbo, &this->camera, identityMatrix));

	this->entityShaders.push_back(new DroneShader(&this->shadowFbo, &this->camera, identityMatrix));

	this->entityShaders.push_back(new WallShader(&this->shadowFbo, &this->camera, identityMatrix));
	this->entityShaders.push_back(new InfinityPlaneShader(&this->shadowFbo, &this->camera, identityMatrix));
	this->entityShaders.push_back(new InfinityPlanePrePassShader(&this->shadowFbo, &this->camera, identityMatrix));

	this->postProcessShaders.push_back(new QuadShader());
	this->postProcessShaders.push_back(new BlurShader());

	this->testShader = new Shader("./Engine/Rendering/Shaders/EntityShader.vert", "./Engine/Rendering/Shaders/EntityShader.frag");
	this->ZprePassShader = new Shader("./Engine/Rendering/Shaders/ZPrepassVert.vert", "./Engine/Rendering/Shaders/ZPrepassFrag.frag");
	this->particleShader = new Shader("./Engine/Particle/Particle.vert", "./Engine/Particle/Particle.frag");
	this->ZprePassShaderInstanced = new Shader("./Engine/Rendering/Shaders/ZPrepassInstanced.vert", "./Engine/Rendering/Shaders/ZPrepassInstanced.frag");
	this->combineShader = new Shader("./Engine/Rendering/Shaders/CombineShader.vert", "./Engine/Rendering/Shaders/CombineShader.frag");

	Display& display = Display::get();

	int width = display.getWidth();
	int height = display.getHeight();
	this->fbo.attachTexture(width, height, AttachmentType::COLOR);
	this->fbo.attachTexture(width, height, AttachmentType::COLOR);
	this->fbo.attachTexture(width, height, AttachmentType::DEPTH);

	float shadowResScale = 4.0f;
	this->shadowFbo.attachTexture(Display::get().getWidth() * 4, Display::get().getHeight() * 4, AttachmentType::DEPTH);

	//Particle init
	ParticleManager::get().init();

	fbo.bind();
	GLenum buf[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, buf);
	fbo.unbind();

	this->uniformBuffers.resize(7);
	for (UniformBuffer* ubo : this->uniformBuffers)
		ubo = nullptr;

	/*
		Set up uniform buffers for shaders
	*/
	this->addUniformBuffer(0, this->testShader->getID(), "Material");
	this->addUniformBuffer(1, this->testShader->getID(), "DirectionalLight");

	for (size_t i = 0; i < this->entityShaders.size(); i++) {
		if (i != SHADERS::INFINITY_PLANE_PREPASS) {
			this->addUniformBuffer(0, this->entityShaders[i]->getID(), "Material");
			this->addUniformBuffer(1, this->entityShaders[i]->getID(), "DirectionalLight");
			this->addUniformBuffer(3, this->entityShaders[i]->getID(), "LightBuffer");
		}
	}
}


Pipeline::~Pipeline()
{
	for(EntityShader* shader : this->entityShaders)
		delete shader;

	for (PostProcessShader* shader : this->postProcessShaders)
		delete shader;

	delete this->ZprePassShader;
	delete this->ZprePassShaderInstanced;
	delete this->testShader;
	delete this->particleShader;
	delete this->combineShader;

	for (UniformBuffer* ubo : this->uniformBuffers)
		delete ubo;

	this->uniformBuffers.clear();

	delete this->quad;
}

Texture* Pipeline::drawParticle()
{
	ParticleManager& pm = ParticleManager::get();

	//Updates vbo if particles are visible also updates particle managers hasVisibleParticles
	if (pm.getParticleCount() != 0) {
		pm.updateBuffer();
	}

	if (pm.hasVisibleParticles()) {
		fbo.bind();
		this->particleShader->bind();
		

		this->particleShader->setUniformMatrix4fv("vp", 1, false, &(this->camera->getVP()[0][0]));
		this->particleShader->setUniform3f("cameraUp", this->camera->getView()[0][1], this->camera->getView()[1][1], this->camera->getView()[2][1]);
		this->particleShader->setUniform3f("cameraRight", this->camera->getView()[0][0], this->camera->getView()[1][0], this->camera->getView()[2][0]);

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);

		pm.getVertexArray()->bind();
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, pm.getParticleCount());

		this->particleShader->unbind();
		fbo.unbind();
	}

	return fbo.getColorTexture(1);
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

void Pipeline::prePassDepthModel(const std::vector<std::pair<RenderingTarget, SHADERS>>& renderingTargets, bool toScreen)
{
	if (!toScreen)
		this->fbo.bind();
	this->prePassDepthOn();
	
	this->ZprePassShaderInstanced->bind();

	for (auto pair : renderingTargets) {
		if (pair.first.prePass) {
			if (pair.second == SHADERS::INFINITY_PLANE) {
				this->entityShaders[SHADERS::INFINITY_PLANE_PREPASS]->bind();

				drawModelPrePassInstanced(pair.first.model);

				this->entityShaders[SHADERS::INFINITY_PLANE_PREPASS]->unbind();
				this->ZprePassShaderInstanced->bind();
			}
			else {

				//Draw renderingList
				this->ZprePassShaderInstanced->setUniformMatrix4fv("vp", 1, false, &(this->camera->getVP()[0][0]));
				drawModelPrePassInstanced(pair.first.model);	
			}
		}
			
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
	//glColorMask(0, 0, 0, 0);

}

void Pipeline::prePassDepthOff()
{
	glDepthFunc(GL_LEQUAL);
	glColorMask(1, 1, 1, 1);
	glDepthMask(GL_FALSE);
}

void Pipeline::createQuad()
{
	this->quad = new Model;

	//Set up post process quad
	std::vector<Vertex>* vertices = new	std::vector<Vertex>();
	std::vector<GLuint>* indicies = new std::vector<GLuint>();

	Vertex vertex;
	vertex.Normal = glm::vec3(0.0);

	vertex.Position = glm::vec3(-1.0, -1.0, 0.0);
	vertex.TexCoords = glm::vec2(0.0, 0.0);
	vertices->push_back(vertex);

	vertex.Position = glm::vec3(1.0, -1.0, 0.0);
	vertex.TexCoords = glm::vec2(1.0, 0.0);
	vertices->push_back(vertex);

	vertex.Position = glm::vec3(1.0, 1.0, 0.0);
	vertex.TexCoords = glm::vec2(1.0, 1.0);
	vertices->push_back(vertex);

	vertex.Position = glm::vec3(-1.0, 1.0, 0.0);
	vertex.TexCoords = glm::vec2(0.0, 1.0);
	vertices->push_back(vertex);

	indicies->push_back(0);
	indicies->push_back(1);
	indicies->push_back(2);
	indicies->push_back(2);
	indicies->push_back(3);
	indicies->push_back(0);

	Mesh* quadMesh = new Mesh(vertices, indicies, 0, this->quad);
	this->quad->addMesh(quadMesh);
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

void Pipeline::drawModelToScreen(const std::vector<std::pair<RenderingTarget, SHADERS>>& renderingTargets)
{
	glEnable(GL_DEPTH_TEST);

	this->entityShaders[SHADERS::DEFAULT]->bind();

	for (auto pair : renderingTargets) {
		if(pair.first.visible)
			drawInstanced(pair.first.model);
	}
	this->entityShaders[SHADERS::DEFAULT]->unbind();
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
	this->testShader->setUniformMatrix4fv("lightMatrix", 1, false, &(lightSpaceMatrix[0][0]));
	this->testShader->setUniform3fv("camPos", 1, &this->camera->getPosition()[0]);

	Texture * shadowTex = getShadowFbo()->getDepthTexture();
	this->testShader->setTexture2D("shadowTex", 1, shadowTex->getID());

	draw(renderingList, this->testShader);

	this->testShader->unbind();
	this->fbo.unbind();

	return this->fbo.getColorTexture(0);
}

Texture * Pipeline::drawModelToTexture(const std::vector<std::pair<RenderingTarget, SHADERS>>& renderingTargets)
{

	this->fbo.bind();
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);

	for (auto pair : renderingTargets) {

		if (pair.first.visible) {
			this->entityShaders[pair.second]->bind();

			drawInstanced(pair.first.model, pair.second);

			this->entityShaders[pair.second]->unbind();
		}
	}

	this->fbo.unbind();

	return this->fbo.getColorTexture(0);
}

void Pipeline::drawTextureToQuad(Texture * tex, SHADERS_POST_PROCESS shader, bool drawToFBO)
{
	PostProcessShader * ppShader = this->postProcessShaders[shader];

	Mesh* mesh = this->quad->getMesh(0);
	mesh->bindVertexArray();
	IndexBuffer& ib = mesh->getIndexBuffer();
	ib.bind();

	if (shader != SHADERS_POST_PROCESS::BLUR_FILTER) {
		glDisable(GL_DEPTH_TEST);

		if (drawToFBO)
			this->fbo.bind();
		ppShader->bind(tex);

		glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0);

		ppShader->unbind();
		if (drawToFBO)
			this->fbo.unbind();
	}
	else {
		BlurShader * blurShader = dynamic_cast<BlurShader*>(ppShader);
		glDisable(GL_DEPTH_TEST);

		this->fbo.bind();
		
		blurShader->bind(tex, true);

		glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0);

		if(!drawToFBO)
			this->fbo.unbind();
		
		blurShader->bind(fbo.getColorTexture(0), false);

		glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0);
		
		blurShader->unbind();

		if (drawToFBO)
			this->fbo.unbind();
	}
}

void Pipeline::calcDirLightDepth(const std::vector<Entity*>& renderingList)
{
	
	
}

void Pipeline::calcDirLightDepthInstanced(const std::vector<std::pair<RenderingTarget, SHADERS>>& renderingTargets)
{
	int displayWidth = Display::get().getWidth();
	int displayHeight = Display::get().getHeight();

	Display::get().updateView(Display::get().getWidth() * 4, (Display::get().getHeight() * 4));

	this->shadowFbo.bind();
	this->prePassDepthOn();
	this->ZprePassShaderInstanced->bind();

	this->ZprePassShaderInstanced->setUniformMatrix4fv("vp", 1, false, &lightManager->getLightMatrix()[0][0]);

	//Draw renderingList
	for (auto pair : renderingTargets) {
		if (pair.first.castShadow)
			drawModelPrePassInstanced(pair.first.model);
	}

	this->ZprePassShaderInstanced->unbind();
	this->prePassDepthOff();
	this->shadowFbo.unbind();

/*#ifdef IMGUI
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
#endif*/

	Display::get().updateView(displayWidth, displayHeight);
}

void Pipeline::updateShaders(const float & dt)
{
	for (EntityShader* shader : this->entityShaders)
		shader->update(dt);
}

void Pipeline::addCurrentLightManager(LightManager * lm)
{
	this->lightManager = lm;
	/*
		Set up Directional Light
	*/
	this->uniformBuffers[1]->setSubData((void*)lightManager->getDirectionalLight(), 32, 0); //no idea how to solve the size issue
	/*
		Set up Point Light
	*/
	struct LightBuffer {
		PointLight pointLights[10];
		int nrOfPointLights;
		glm::vec3 padding;
	} lightBuffer;

	lightBuffer.nrOfPointLights = lightManager->getNrOfPointLights();

	for (int i = 0; i < lightManager->getNrOfPointLights(); i++) {
		lightBuffer.pointLights[i] = *lightManager->getPointLights()->at(i);
	}

	this->uniformBuffers[3]->setSubData((void*)(&lightBuffer), sizeof(lightBuffer), 0);
	this->entityShaders[DEFAULT]->updateLightMatrixData(lightManager->getLightMatrixPointer());
	this->entityShaders[DRONE_SHADER]->updateLightMatrixData(lightManager->getLightMatrixPointer());
	this->entityShaders[WALL]->updateLightMatrixData(lightManager->getLightMatrixPointer());
	this->entityShaders[INFINITY_PLANE]->updateLightMatrixData(lightManager->getLightMatrixPointer());
	this->entityShaders[INFINITY_PLANE_PREPASS]->updateLightMatrixData(lightManager->getLightMatrixPointer());

}

void Pipeline::setActiveCamera(Camera * camera)
{
	this->camera = camera;
}

Camera * Pipeline::getActiveCamera()
{
	return this->camera;
}

void Pipeline::setWallPoints(const std::vector<glm::vec3>& wallPoints, const std::vector<int>& wallGroupsIndex)
{
	if (wallPoints.empty())
		return;

	EntityShader* eShader = this->entityShaders[SHADERS::INFINITY_PLANE];
	InfinityPlaneShader* infPlaneShader = dynamic_cast<InfinityPlaneShader*>(eShader);
	if (infPlaneShader)
		this->addUniformBuffer(2, infPlaneShader->getID(), "WallPoints");

	eShader = this->entityShaders[SHADERS::INFINITY_PLANE_PREPASS];
	InfinityPlanePrePassShader* infPlanePrePassShader = dynamic_cast<InfinityPlanePrePassShader*>(eShader);
	if (infPlanePrePassShader)
		this->addUniformBuffer(2, infPlanePrePassShader->getID(), "WallPoints");


	if (infPlaneShader != nullptr)
	{
		struct WallPointsData
		{
			glm::vec4 points[96];
			int size;
			int groupSize;
			int a;
			int b;
		} data;
		data.size = wallPoints.size();
		data.groupSize = wallGroupsIndex.size();
		int value = wallGroupsIndex[0];
		int index = 0;
		for (int i = 0; i < data.size; i++) {
			if (value - i == 0)
				value += wallGroupsIndex[++index];
			data.points[i] = glm::vec4(wallPoints[i], (float)(wallGroupsIndex[index]));
		}
		this->uniformBuffers[2]->setSubData((void*)&data, sizeof(WallPointsData), 0);
	}
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

void Pipeline::drawInstanced(Model * model, SHADERS shader)
{

	for (size_t i = 0; i < model->meshCount(); i++)
	{
		Mesh* mesh = model->getMesh(i);

		unsigned int materialIndex = mesh->getMaterialIndex();
		Material& material = model->getMaterial(materialIndex);

		this->uniformBuffers[0]->setSubData((void*)&material, sizeof(material) - sizeof(material.textures), 0);

		for (Texture* texture : material.textures) {
			this->entityShaders[shader]->updateMeshData(texture->getID());
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
	this->fbo.updateDimensions(1, event->width, event->height);
}

Texture* Pipeline::combineTextures(Texture * sceen, Texture * particles)
{
	fbo.bind();
	glDisable(GL_DEPTH_TEST);

	this->combineShader->bind();
	this->combineShader->setTexture2D("tex", 0, sceen->getID());
	this->combineShader->setTexture2D("particles", 1, particles->getID());

	Mesh* mesh = this->quad->getMesh(0);
	mesh->bindVertexArray();
	IndexBuffer& ib = mesh->getIndexBuffer();
	ib.bind();
	glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0);

	this->combineShader->unbind();
	fbo.unbind();

	return fbo.getColorTexture(0);
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
