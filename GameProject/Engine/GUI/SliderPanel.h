#pragma once

#include <functional>

#include "Panel.h"
#include "Engine/Events/Events.h"

class Button;

class SliderPanel : public Panel
{
public:
	SliderPanel();
	~SliderPanel();

	// Sets the color of the slider (default purple)
	void setSliderColor(const glm::vec3& color);

	// Set color of slider background
	void setBackgroundColor(const glm::vec3& color);

	// Set callback for the slider
	void setSliderCallback(const std::function<void(float)>& callback);

	// Get percent of slider
	float getSliderFactor();

	// Set slider factor
	void setSliderFactor(float factor);

private:
	Panel* slider;
	Panel* text;
	Panel* background;
	Button* btn;

	std::function<void(float)> callback;

	glm::vec3 sliderColor;
	glm::vec3 backgroundColor;
	glm::uvec2 sliderSize;

	void mouseCallback();
	void createBackground();
	void createSlider();
	void createButton();
	void createText();
	void updateText();

	int offset;
};