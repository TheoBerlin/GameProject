#include "Renderer.h"

#include "../Entity/Entity.h"
#include "../AssetManagement/Mesh.h"
#include "GLAbstraction/Texture.h"

Renderer::Renderer()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

Renderer::~Renderer()
{

}

void Renderer::setActiveCamera(Camera * camera)
{
	this->pipeline.setActiveCamera(camera);
}

void Renderer::push(Entity * entity)
{
	this->renderingList.push_back(entity);
}

void Renderer::drawAll()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
		Z-prepass stage
	*/
	this->pipeline.prePassDepth(this->renderingList);

	///*
	//	Drawing stage with pre existing depth buffer to texture
	//*/
	postProcessTexture = this->pipeline.drawToTexture(this->renderingList);

	tex = this->pipeline.drawParticle();
	///*
	//	Draw texture of scene to quad for postprocessing
	//*/
	this->pipeline.drawTextureToQuad(postProcessTexture, tex);



	this->renderingList.clear();
}

void Renderer::initInstancing()
{
	std::vector<Model*> models = ModelLoader::getModels();

	for (Model* model : models) {
		model->initInstancing();
	}
}

void Renderer::updateInstancingData(Model * model)
{
	model->updateInstancingData();
}

void Renderer::drawAllInstanced()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pipeline.getFbo()->bind();
	GLenum buf[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, buf);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	std::vector<Model*> models = ModelLoader::getModels();
	for (Model* model : models) {
		this->pipeline.drawInstanced(model);
	}

	pipeline.drawParticle();
	
	postProcessTexture = pipeline.getFbo()->getColorTexture(0);
	tex = pipeline.getFbo()->getColorTexture(1);

	pipeline.getFbo()->unbind();

	this->pipeline.drawTextureToQuad(postProcessTexture, tex);
}
