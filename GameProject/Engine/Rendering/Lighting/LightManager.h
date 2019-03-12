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
	void setShadowReScale(float reScale);
	float getShadowHeightScaled() const;
	float getShadowWidthScaled() const;
	PointLight * createPointLight(glm::vec4 position, glm::vec4 intensity, int distance);
	std::vector<PointLight*> * getPointLights();
	unsigned int getNrOfPointLights() const;
	void removePointLight(int index);
	DirectionalLight * createDirectionalLight(glm::vec4 direction, glm::vec4 intensity);
	DirectionalLight * getDirectionalLight() const;
	glm::mat4 getLightMatrix() const;
	glm::mat4 * getLightMatrixPointer();
	void updatePointLight(int index, glm::vec4 position, glm::vec4 intensity, int distance);

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
	int shadowHeight;
	float shadowReScale;
};