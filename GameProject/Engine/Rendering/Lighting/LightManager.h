#pragma once
#include "PointLight.h"
#include "DirectionalLight.h"
#include "Utils/Settings.h"
#include <vector>
#include "../../Config.h"

struct Level;
#define MAX_POINT_LIGHT 10

class LightManager
{
public:
	LightManager();
	~LightManager();
	
	PointLight * createPointLight(glm::vec4 position, glm::vec4 intensity, int distance);
	std::vector<PointLight*> * getPointLights();
	int getNrOfPointLights();

	DirectionalLight * createDirectionalLight(glm::vec4 direction, glm::vec4 intensity, Level* level);
	DirectionalLight * getDirectionalLight();
	glm::mat4 getShadowMatrix();
	glm::mat4 * getShadowMatrixPointer();

	float getShadowWidth() const;
	float getShadowHeight() const;

#ifdef ENABLE_SHADOW_BOX
	struct ShadowBox {
		glm::vec3 tl;
		glm::vec3 tr;
		glm::vec3 bl;
		glm::vec3 br;

		glm::vec3 ftl;
		glm::vec3 ftr;
		glm::vec3 fbl;
		glm::vec3 fbr;

		glm::vec3 e1;
		glm::vec3 e2;
		glm::vec3 e3;

		glm::vec3 p;
	};

	void drawDebugBox();

	void toggleDrawing(KeyEvent* evnt);
#else
	void drawDebugBox() {}
#endif

	unsigned int getNrOfPointLights() const;
	void removePointLight(int index);
	void updatePointLight(int index, glm::vec4 position, glm::vec4 intensity, int distance);

private:
	void calcShadowProjection(float width, float height, float near=5.0f, float far = 50.0f);
	void calcShadowMatrix(Level* level);

	std::vector<PointLight*> pointLights;

	DirectionalLight * dirLight;
	bool dirLightExist = false;

	glm::mat4 shadowMatrix;

	float shadowWidth;
	float orthoWidth;
	float orthoHeight;
	float shadowResolutionFactor;
	glm::mat4 shadowProjection;

#ifdef ENABLE_SHADOW_BOX
	// Debug
	bool activateShadowBox;
	ShadowBox shadowBox;
#endif
};