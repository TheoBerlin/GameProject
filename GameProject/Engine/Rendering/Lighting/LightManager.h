#pragma once
#include "PointLight.h"
#include "DirectionalLight.h"
#include "Utils/Settings.h"
#include <vector>

class LightManager
{
public:
	LightManager();
	~LightManager();
	float getShadowResolutionFactor() const;
	PointLight * createPointLight(glm::vec4 position, glm::vec4 intensity, int distance);
	std::vector<PointLight*> * getPointLights();
	int getNrOfPointLights();

	DirectionalLight * createDirectionalLight(glm::vec4 direction, glm::vec4 intensity);
	DirectionalLight * getDirectionalLight();
	glm::mat4 getShadowMatrix();
	glm::mat4 * getShadowMatrixPointer();

private:
	void calcShadowProjection(float width, float height, float near=5.0f, float far = 50.0f);
	void calcShadowMatrix();

	std::vector<PointLight*> pointLights;

	DirectionalLight * dirLight;
	bool dirLightExist = false;

	glm::mat4 shadowMatrix;

	float orthoWidth;
	float orthoHeight;
	int shadowWidth;
	int shadowHeight;
	float shadowResolutionFactor;
	glm::mat4 shadowProjection;
};