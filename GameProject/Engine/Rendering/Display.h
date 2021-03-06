#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ft2build.h"
#include FT_FREETYPE_H

#include <string>

class LineRenderer;
class GUIRenderer;
class Renderer;
class Display
{
public:
	static Display& get();
	void init(int width, int height, const std::string& title, bool fullscreen = false);

	void updateView(int width, int height);

	bool isOpen() const;
	void startFrame();
	void endFrame();

	void setTitle(const std::string& title);
	void setTitleSuffix(const std::string& suffix);

	void toggleFullscreen();

	int getWidth() const;
	int getHeight() const;
	float getRatio() const;
	float getPixelXScale() const;
	float getPixelYScale() const;
	std::string getTitle() const;

	FT_Library& getFTLibrary();

	GLFWwindow* getWindowPtr();
	Renderer& getRenderer();
	GUIRenderer& getGUIRenderer();
	LineRenderer& getLineRenderer();

	~Display();

private:
	Display() = default;
	Display(const Display& other) = delete;

	static void errorCallback(int error, const char* description);
	static void resizeCallback(GLFWwindow* window, int width, int height);
	static void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

	GLFWwindow* window;
	std::string title;
	int width;
	int height;
	bool fullscreen;

	FT_Library ftLibrary;

	Renderer* renderer;
	GUIRenderer* guiRenderer;
	LineRenderer* lineRenderer;
};
