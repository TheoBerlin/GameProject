#include "Renderer.h"

#include "../Entity/Entity.h"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::setActiveCamera(Camera * camera)
{
	this->activeCamera = camera;
}

void Renderer::draw(Entity * entity)
{
	
}
