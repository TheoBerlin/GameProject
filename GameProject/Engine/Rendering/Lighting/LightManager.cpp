#include "LightManager.h"
#include "Engine/Rendering/Display.h"
#include "Utils/Logger.h"
#include "glm/gtc/matrix_transform.hpp"

#include <Game/Level/Level.h>
#include "Engine/Rendering/LineRenderer.h"

LightManager::LightManager()
{
	this->shadowResolutionFactor = Settings::get().getShadowResolutionFactor();
	this->orthoWidth = 0.f;
	this->orthoHeight = 0.f;
	this->shadowWidth = Settings::get().getScreenWidth()*this->shadowResolutionFactor;

	EventBus::get().subscribe(this, &LightManager::toggleDrawing);
	this->activateShadowBox = false;
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

DirectionalLight * LightManager::createDirectionalLight(glm::vec4 direction, glm::vec4 intensity, Level* level)
{
	if (!dirLightExist) {
		dirLight = new DirectionalLight(direction, intensity);
		dirLightExist = true;
		calcShadowMatrix(level);
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

float LightManager::getShadowWidth() const
{
	return this->shadowWidth;
}

float LightManager::getShadowHeight() const
{
	return this->shadowWidth * (this->orthoWidth/this->orthoHeight);
}

void LightManager::drawDebugBox()
{
	if (this->activateShadowBox)
	{
		LineRenderer& lineRenderer = Display::get().getLineRenderer();
		glm::vec3 color(0.f, 0.f, 1.0f);
		lineRenderer.drawLine(this->shadowBox.tl, this->shadowBox.ftl, color);
		lineRenderer.drawLine(this->shadowBox.tr, this->shadowBox.ftr, color);
		lineRenderer.drawLine(this->shadowBox.bl, this->shadowBox.fbl, color);
		lineRenderer.drawLine(this->shadowBox.br, this->shadowBox.fbr, color);

		lineRenderer.drawLine(this->shadowBox.tl, this->shadowBox.tr, color);
		lineRenderer.drawLine(this->shadowBox.tr, this->shadowBox.br, color);
		lineRenderer.drawLine(this->shadowBox.br, this->shadowBox.bl, color);
		lineRenderer.drawLine(this->shadowBox.bl, this->shadowBox.tl, color);

		lineRenderer.drawLine(this->shadowBox.ftl, this->shadowBox.ftr, color);
		lineRenderer.drawLine(this->shadowBox.ftr, this->shadowBox.fbr, color);
		lineRenderer.drawLine(this->shadowBox.fbr, this->shadowBox.fbl, color);
		lineRenderer.drawLine(this->shadowBox.fbl, this->shadowBox.ftl, color);

		lineRenderer.drawLine(this->shadowBox.p, this->shadowBox.p + this->shadowBox.e1, { 1.f, 0.f, 0.f });
		lineRenderer.drawLine(this->shadowBox.p, this->shadowBox.p + this->shadowBox.e2, { 0.f, 1.f, 0.f });
		lineRenderer.drawLine(this->shadowBox.p, this->shadowBox.p + this->shadowBox.e3, { 0.f, 0.f, 1.f });
	}
}

void LightManager::toggleDrawing(KeyEvent * evnt)
{
	if (evnt->key == GLFW_KEY_F3 && evnt->action == GLFW_PRESS) {
		this->activateShadowBox = !this->activateShadowBox;
	}
}

void LightManager::calcShadowProjection(float width, float height, float near, float far)
{
	this->shadowProjection = glm::ortho(-((float)width * 0.5f), ((float)width * 0.5f), -((float)height * 0.5f), ((float)height * 0.5f), near, far);
}

void LightManager::calcShadowMatrix(Level* level)
{
	glm::vec3 dir = glm::vec3(dirLight->getDirection());
	dir = {0.0f, -1.0f, 0.1f};
	glm::mat4 shadowView = glm::lookAt(glm::vec3(0.0f), dir, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 e1 = { shadowView[0].x, shadowView[1].x, shadowView[2].x };
	glm::vec3 e2 = { shadowView[0].y, shadowView[1].y, shadowView[2].y };
	glm::vec3 e3 = { shadowView[0].z, shadowView[1].z, shadowView[2].z };
	Utils::AABB aabb = level->levelStructure->createBoundingBox(e1, e2, e3);

	this->shadowBox.e1 = e1 * aabb.size.x*.5f;
	this->shadowBox.e2 = e2 * aabb.size.y*.5f;
	this->shadowBox.e3 = e3 * aabb.size.z*.5f;

	this->orthoWidth = aabb.size.x;
	this->orthoHeight = aabb.size.y;
	calcShadowProjection(aabb.size.x, aabb.size.y, 0.01f,  aabb.size.z);

	glm::vec3 pos = aabb.pos + e3 * aabb.size.z*.5f;
	this->shadowBox.p = pos;

	glm::mat4 translation(1.0f);
	translation = glm::translate(translation, -pos);
	this->shadowMatrix = this->shadowProjection * shadowView * translation;

	this->shadowBox.tl = aabb.pos + e3 * aabb.size.z*.5f - e1 * aabb.size.x * .5f - e2 * aabb.size.y*.5f;
	this->shadowBox.tr = aabb.pos + e3 * aabb.size.z*.5f + e1 * aabb.size.x * .5f - e2 * aabb.size.y*.5f;
	this->shadowBox.bl = aabb.pos + e3 * aabb.size.z*.5f - e1 * aabb.size.x * .5f + e2 * aabb.size.y*.5f;
	this->shadowBox.br = aabb.pos + e3 * aabb.size.z*.5f + e1 * aabb.size.x * .5f + e2 * aabb.size.y*.5f;

	this->shadowBox.ftl = aabb.pos - e3 * aabb.size.z*.5f - e1 * aabb.size.x * .5f - e2 * aabb.size.y*.5f;
	this->shadowBox.ftr = aabb.pos - e3 * aabb.size.z*.5f + e1 * aabb.size.x * .5f - e2 * aabb.size.y*.5f;
	this->shadowBox.fbl = aabb.pos - e3 * aabb.size.z*.5f - e1 * aabb.size.x * .5f + e2 * aabb.size.y*.5f;
	this->shadowBox.fbr = aabb.pos - e3 * aabb.size.z*.5f + e1 * aabb.size.x * .5f + e2 * aabb.size.y*.5f;
}
