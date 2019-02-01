#pragma once

class Entity;
class Renderer
{
public:
	Renderer();
	virtual ~Renderer();

	void draw(Entity* entity);

private:

};