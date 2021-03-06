#pragma once

#include "glm/gtc/matrix_transform.hpp"
#include <random>

// Enable logging.
#define LOG_ENABLE_PRINT

// Debug openGL
#define GL_DEBUG_LAYER

// Define IMGUI
#define IMGUI

//Debug defines
#define ENABLE_PATH_VISUALIZERS false
#define ENABLE_SHADOW_BOX

// Common defines
#define EPSILON 0.0000001

// Window defines
#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720
#define DEFAULT_TITLE "Little Game Project"

#define FRAMES_PER_SECOND 30

// Camera defines
#define STARTING_FOV 75;
#define ZFAR 100.0f;
#define ZNEAR 0.1f;
static std::mt19937 ran;
const glm::vec3 GLOBAL_UP_VECTOR = glm::vec3(0.0f, 1.0f, 0.0f);
