#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Catch/catch.hpp>

#include "Engine/AssetManagement/ModelLoader.h"

TEST_CASE("ModelLoader loads assets") {
	Model* cube = ModelLoader::loadModel("../Game/assets/cube.obj");
	REQUIRE(cube != nullptr);

	SECTION("No duplicate models are created") {
		Model* duplicateAttempt = ModelLoader::loadModel("../Game/assets/cube.obj");

		REQUIRE(cube == duplicateAttempt);
	}

	SECTION("Meshes are created") {
		REQUIRE(cube->meshCount() > 0);

		SECTION("Each mesh has a material") {
			for (unsigned int i = 0; i < cube->meshCount(); i += 1) {
				unsigned short materialIndex = cube->getMesh(i)->getMaterialIndex();

				REQUIRE((materialIndex < cube->meshCount() && materialIndex >= 0));
			}

		}
	}
}
