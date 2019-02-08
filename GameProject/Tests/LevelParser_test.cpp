#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Catch/catch.hpp>
#include "Engine/Level/LevelParser.h"

TEST_CASE("LevelParser Test") {
	EntityManager EM;
	LevelParser parser;

	parser.readEntity("../Engine/Level/level.json", &EM);
}