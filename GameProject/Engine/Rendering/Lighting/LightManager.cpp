#include "LightManager.h"
#include "Engine/Rendering/Display.h"
#include "Utils/Logger.h"
#include "glm/gtc/matrix_transform.hpp"

LightManager::LightManager()
{
	this->shadowReScale = Settings::get().getShadowReScale();
	this->shadowHeight = Display::get().getHeight();
	this->shadowWidth = Display::get().getWidth();
	this->orthoHeight = 80.0f * Display::get().getRatio();
	this->orthoWidth = 80.0f; //fix this so that the class gets this info relative to input
	this->shadowPosition = glm::vec3(2.0f, 10.0f, 2.0f);
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
	this->shadowReScale = reScale;
}

float LightManager::getShadowHeightScaled()
{
	return this->shadowHeight * shadowReScale;
}

float LightManager::getShadowWidthScaled()
{
	return this->shadowWidth * shadowReScale;
}

PointLight * LightManager::createPointLight(glm::vec4 position, glm::vec4 intensity, int distance)
{
	if (pointLights.size() < 10) {
		PointLight *  pointLight = new PointLight(position, intensity, distance);
		pointLights.push_back(pointLight);
		return pointLight;
	}
	else {
		LOG_ERROR("Maximum number of pointlights already added");
	}
	return nullptr;
}

std::vector<PointLight*> * LightManager::getPointLights()
{
	return &this->pointLights;
}

int LightManager::getNrOfPointLights()
{
	return pointLights.size();
}

void LightManager::removePointLight(int index)
{
	if ((unsigned int)index <= pointLights.size() && index >= 0) {
		delete pointLights[index];
		pointLights[index] = pointLights[pointLights.size()-1];
		pointLights.pop_back();
	}
	else {
		LOG_ERROR("Index out of range in list of Pointlights");
	}
}

DirectionalLight * LightManager::createDirectionalLight(glm::vec4 direction, glm::vec4 intensity)
{
	if (!dirLightExist) {
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

glm::mat4 * LightManager::getLightMatrixPointer()
{
	return &this->lightMatrix;
}

void LightManager::updatePointLight(int index, glm::vec4 position, glm::vec4 intensity, int distance)
{
	pointLights.at(index)->setPosition(position);
	pointLights.at(index)->setIntensity(intensity);
	pointLights.at(index)->setDistance(distance);
}

void LightManager::calcLightMatrix()
{
	glm::mat4 lightProjection = glm::ortho(-((float)orthoWidth / 2.0f), ((float)orthoWidth / 2.0f), -((float)orthoHeight / 2.0f), ((float)orthoHeight / 2.0f), 0.1f, 100.0f);
	glm::mat4 lightView = glm::lookAt(glm::vec3(shadowPosition), glm::vec3(dirLight->getDirection()), glm::vec3(0.0f, 1.0f, 0.0f));
	lightMatrix = lightProjection * lightView;
}
