#pragma once

#include <vector>

class Panel
{
public:
	Panel();
	virtual ~Panel();


private:
	std::vector<Panel*> panels;
};
