#include "PointLight.h"

PointLight::PointLight(glm::vec4 position, glm::vec4 intensity, int distance)
{
	/*The distance is predefined to certain values,
	values to choose are 7, 13, 20, 32, 50, 65, 100, 200 and 325.
	Anything that does not match these values will get a default vaule
	(the distances will be considered as 600).*/

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
	case 20:
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
	case 160:
		this->linear = 0.027f;
		this->quadratic = 0.0028f;
		break;
	case 200:
		this->linear = 0.022f;
		this->quadratic = 0.0019f;
		break;
	case 325:
		this->linear = 0.014f;
		this->quadratic = 0.0007f;
		break;
	default:
		this->linear = 0.007f;
		this->quadratic = 0.0002f;
		break;
	}
}

PointLight::PointLight()
{
	this->position = glm::vec4(0.0f);
	this->intensity = glm::vec4(1.0f);
	this->constant = 1.0f;
	this->linear = 0.027f;
	this->quadratic = 0.0028f;
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
