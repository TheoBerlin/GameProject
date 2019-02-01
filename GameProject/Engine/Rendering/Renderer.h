#pragma once

#include "../Components/Camera.h"

class Entity;
class Renderer
{
public:
	Renderer();
	virtual ~Renderer();

	void setActiveCamera(Camera* camera);

	void draw(Entity* entity);

private:
	Camera* activeCamera;
};