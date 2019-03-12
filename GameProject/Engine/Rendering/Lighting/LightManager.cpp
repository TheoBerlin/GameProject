#include "LightManager.h"
#include "Engine/Rendering/Display.h"
#include "Utils/Logger.h"
#include "glm/gtc/matrix_transform.hpp"

LightManager::LightManager()
{
	this->shadowResolutionFactor = Settings::get().getShadowResolutionFactor();
	this->shadowHeight = Display::get().getHeight();
	this->shadowWidth = Display::get().getWidth();
	this->orthoHeight = 20.0f * Display::get().getRatio();
	this->orthoWidth = 20.0f; //fix this so that the class gets this info relative to input
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

float LightManager::getShadowResolutionFactor() const
{
	return this->shadowResolutionFactor;
}

PointLight * LightManager::createPointLight(glm::vec4 position, glm::vec4 intensity, int distance)
{
	PointLight *  pointLight = new PointLight(position, intensity, distance);
	pointLights.push_back(pointLight);
	return pointLight;
}

std::vector<PointLight*> * LightManager::getPointLights()
{
	return &this->pointLights;
}

int LightManager::getNrOfPointLights()
{
	return pointLights.size();
}

DirectionalLight * LightManager::createDirectionalLight(glm::vec4 direction, glm::vec4 intensity)
{
	if (!dirLightExist) {
		dirLight = new DirectionalLight(direction, intensity);
		dirLightExist = true;
		calcShadowProjection(this->orthoWidth, this->orthoHeight, 5.0f, 35.0f);
		calcShadowMatrix();
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

glm::mat4 LightManager::getShadowMatrix()
{
	return this->shadowMatrix;
}

glm::mat4 * LightManager::getShadowMatrixPointer()
{
	return &this->shadowMatrix;
}

void LightManager::calcShadowProjection(float width, float height, float near, float far)
{
	this->shadowProjection = glm::ortho(-((float)width * 0.5f), ((float)width * 0.5f), -((float)height * 0.5f), ((float)height * 0.5f), near, far);
}

void LightManager::calcShadowMatrix()
{
	glm::vec3 dir = glm::vec3(dirLight->getDirection());
	dir = {0.0f, -1.0f, 0.1f};
	glm::mat4 shadowView = glm::lookAt(glm::vec3(0.0f), dir, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 t(1.0f);
	t = glm::translate(t, {0.0f, -20.0f, 15.0f});
	this->shadowMatrix = this->shadowProjection * shadowView * t;
}
