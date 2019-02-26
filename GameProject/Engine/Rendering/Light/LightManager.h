#pragma once
#include "Light.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "Utils/Settings.h"
#include <vector>

class LightManager
{
public:
	LightManager();
	~LightManager();
	void setShadowReScale(float reScale);
	float getShadowHeightScaled();
	float getShadowWidthScaled();
	PointLight * createPointLight();
	DirectionalLight * createDirectionalLight(glm::vec4 direction, glm::vec4 inensity);
	DirectionalLight * getDirectionalLight();
	glm::mat4 getLightMatrix();
private:
	std::vector<PointLight*> pointLights; 
	DirectionalLight * dirLight;
	bool dirLightExist = false;
	glm::vec3 shadowPosition;
	glm::mat4 lightMatrix;

	void calcLightMatrix();


	float orthoWidth;
	float orthoHeight;
	int shadowWidth;
	int shadowHeith;
	float shadowReScale;
};