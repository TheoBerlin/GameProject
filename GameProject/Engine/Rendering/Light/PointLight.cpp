#include "PointLight.h"

PointLight::PointLight(glm::vec4 position, glm::vec4 intensity, int distance) 
{
	/*The distance is predefined to certain values, 
	values to choose are 7, 13, 20, 32, 50, 65, 100.
	Anything that does not match these values will get a default vaule 
	(the distances will be considered as aboce 100).*/

	this->position = position;
	this->intensity = intensity;
	this->constant = 1.0f;
	switch (distance) {
	case 7:
		this->linear = 0.7f;
		this->quadratic = 1.8f;
		break;
	case 13:
		this->linear = 0.35f;
		this->quadratic = 0.44f;
		break;
	case 29:
		this->linear = 0.22f;
		this->quadratic = 0.20f;
		break;
	case 32:
		this->linear = 0.14f;
		this->quadratic = 0.07f;
		break;
	case 50:
		this->linear = 0.09f;
		this->quadratic = 0.032f;
		break;
	case 65:
		this->linear = 0.07f;
		this->quadratic = 0.017f;
		break;
	case 100:
		this->linear = 0.045f;
		this->quadratic = 0.0075f;
		break;
	default:
		this->linear = 0.027f;
		this->quadratic = 0.0028f;
		break;

	}
}

PointLight::~PointLight()
{
}

glm::vec4 PointLight::getPosition()
{
	return this->position;
}

glm::vec4 PointLight::getIntensity()
{
	return this->intensity;
}
