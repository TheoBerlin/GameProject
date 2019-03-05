#include "Display.h"

#include "Utils/Logger.h"
#include "../Events/EventBus.h"
#include "../Events/Events.h"
#include "../Imgui/imgui.h"
#include "../Imgui/imgui_impl_glfw.h"
#include "../Imgui/imgui_impl_opengl3.h"

#include "Renderer.h"
#include "GUIRenderer.h"

Display & Display::get()
{
	static Display display;
	return display;
}

void Display::updateView(int width, int height)
{
	this->width = width;
	this->height = height;
	glViewport(0, 0, this->width, this->height);
}

bool Display::isOpen() const
{
	return !glfwWindowShouldClose(this->window);
}

void Display::startFrame()
{
	#ifdef IMGUI
	//Create new frame for ImGui
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	#endif /* IMGUI */
}

void Display::endFrame()
{
	#ifdef IMGUI
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	#endif /* IMGUI */

	glfwSwapBuffers(this->window);
	glfwPollEvents();
}

void Display::setTitle(const std::string & title)
{
	this->title = title;
	glfwSetWindowTitle(this->window, title.c_str());
}

void Display::setTitleSuffix(const std::string & suffix)
{
	glfwSetWindowTitle(this->window, (this->title + suffix).c_str());
}

int Display::getWidth() const
{
	return this->width;
}

int Display::getHeight() const
{
	return this->height;
}

float Display::getRatio() const
{
	return (float)this->width / (float)this->height;
}

float Display::getPixelXScale() const
{
	return 1.0f / (float)this->width;
}

float Display::getPixelYScale() const
{
	return 1.0f / (float)this->height;
}

std::string Display::getTitle() const
{
	return this->title;
}

FT_Library & Display::getFTLibrary()
{
	return this->ftLibrary;
}

GLFWwindow * Display::getWindowPtr()
{
	return this->window;
}

Renderer & Display::getRenderer()
{
	return *this->renderer;
}

GUIRenderer & Display::getGUIRenderer()
{
	return *this->guiRenderer;
}

Display::~Display()
{
	#ifdef IMGUI
	//Cleanup for ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	#endif /* IMGUI */

	delete this->renderer;
	delete this->guiRenderer;

	FT_Done_FreeType(this->ftLibrary);

	glfwDestroyWindow(this->window);
	glfwTerminate();
}

void Display::errorCallback(int error, const char * description)
{
	LOG_ERROR("Glfw not initialized! %s", description);
}

void Display::resizeCallback(GLFWwindow * window, int width, int height)
{
	if (width != 0 || height != 0)
	{
		Display::get().updateView(width, height);
		EventBus::get().publish(&WindowResizeEvent(width, height));
	}
}

void Display::glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void * userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	LOG_START_COLOR_PASS(Logger::CONSOLE_COLOR::RED);

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;

	LOG_END_COLOR_PASS();
}

void Display::init(int width, int height, const std::string& title)
{
	this->width = width;
	this->height = height;
	this->title = title;

	if(!glfwInit()) 
	{
		LOG_ERROR("Failed to initialize GLFW!");
		exit(EXIT_FAILURE);
	}

	glfwSetErrorCallback(errorCallback);

	//glfwWindowHint(GLFW_SAMPLES, 4);// 4x antialiasing
	// OpenGL 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Modern opengl

	this->window = glfwCreateWindow(this->width, this->height, this->title.c_str(), NULL, NULL);
	if (this->window == NULL)
	{
		LOG_ERROR("Failed to create GLFW window!");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(this->window);
	//glfwSwapInterval(1); // Enable vsync

	// --------------- INIT GLEW ---------------
	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK)
	{
		LOG_ERROR("Failed to initialize GLEW!");
		exit(EXIT_FAILURE);
	}
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	LOG_PRINT("Renderer: %s\n", renderer);
	LOG_PRINT("OpenGL version %s\n", version);

	glViewport(0, 0, this->width, this->height);

	glfwSetWindowSizeCallback(this->window, resizeCallback);

#ifdef GL_DEBUG_LAYER
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(glDebugOutput, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif

	#ifdef IMGUI
	// ----------------- INIT ImGui -----------------
	//Context and init
	const char* glsl_version = "#version 430";
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(this->window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	#endif /* IMGUI */

	// ----------------- INIT FreeType-----------------
	if (FT_Init_FreeType(&this->ftLibrary))
	{
		LOG_ERROR("Failed to initialize FreeType library");
		exit(EXIT_FAILURE);
	}

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	this->renderer = new Renderer();
	this->guiRenderer = new GUIRenderer();
}
