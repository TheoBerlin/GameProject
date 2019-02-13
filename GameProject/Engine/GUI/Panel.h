#pragma once

#include <vector>
#include "../Rendering/GLAbstraction/Texture.h"

class Panel
{
public:
	Panel();
	virtual ~Panel();


private:
	std::vector<Panel*> panels;
	Texture* bakedTexture;
};
