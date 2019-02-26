#include "LightManager.h"
#include "Engine/Rendering/Display.h"
#include "Utils/Logger.h"

LightManager::LightManager()
{
	this->shadowReScale = Settings::get().getShadowReScale();
	this->shadowHeith = Display::get().getHeight();
	this->shadowWidth = Display::get().getWidth();
	this->orthoHeight = 20.0f * Display::get().getRatio();
	this->orthoWidth = 20.0f; //fix this so that the class gets this info relative to input
	this->shadowPosition = glm::vec3(0.0f, 10.0f, 10.0f);
}


LightManager::~LightManager()
{
	for (unsigned int i = 0; i < pointLights.size(); i++) {
		delete pointLights[i];
		pointLights[i] = nullptr;
	}
	pointLights.clear();
	delete this->dirLight;
}

void LightManager::setShadowReScale(float reScale)
{
	Settings::get().setShadowReScale(reScale);
}

float LightManager::getShadowHeightScaled()
{
	return this->shadowHeith * shadowReScale;
}

float LightManager::getShadowWidthScaled()
{
	return this->shadowWidth * shadowReScale;
}

PointLight * LightManager::createPointLight(glm::vec4 position, glm::vec4 intensity)
{
	PointLight *  pointLight = new PointLight(position, intensity);
	pointLights.push_back(pointLight);
	return pointLight;
}

DirectionalLight * LightManager::createDirectionalLight(glm::vec4 direction, glm::vec4 intensity)
{
	if (dirLightExist == false) {
		dirLight = new DirectionalLight(direction, intensity);
		dirLightExist = true;
		calcLightMatrix();
		return dirLight;
	}
	else {
		LOG_WARNING("DirectionalLight already exist for this LightManager");
	}
	return nullptr;
}

DirectionalLight * LightManager::getDirectionalLight()
{
	return this->dirLight;
}

glm::mat4 LightManager::getLightMatrix()
{
	return this->lightMatrix;
}

void LightManager::calcLightMatrix()
{
	glm::mat4 lightProjection = glm::ortho(-((float)orthoWidth / 2.0f), ((float)orthoWidth / 2.0f), -((float) orthoHeight / 2.0f), ((float)orthoHeight / 2.0f), 0.1f, 100.0f);
	glm::mat4 lightView = glm::lookAt(glm::vec3(shadowPosition), glm::vec3(dirLight->getDirection()), glm::vec3(0.0f, 1.0f, 0.0f));
	lightMatrix = lightProjection * lightView;
}
