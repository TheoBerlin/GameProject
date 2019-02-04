#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

class Renderer;
class Display
{
public:
	static Display& get();

	void updateView(int width, int height);

	bool isOpen() const;
	void startFrame();
	void endFrame();

	void setTitle(const std::string& title);
	void setTitleSuffix(const std::string& suffix);

	int getWidth() const;
	int getHeight() const;
	float getRatio() const;
	float getPixelXScale() const;
	float getPixelYScale() const;
	std::string getTitle() const;

	GLFWwindow* getWindowPtr();
	Renderer& getRenderer();

	~Display();

private:
	static void errorCallback(int error, const char* description);
	static void resizeCallback(GLFWwindow* window, int width, int height);
	void init(int width, int height, const std::string& title);

	GLFWwindow* window;
	std::string title;
	int width;
	int height;

	Renderer* renderer;
};
