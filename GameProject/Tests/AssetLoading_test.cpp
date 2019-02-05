#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Catch/catch.hpp>

#include "Engine/AssetManagement/ModelLoader.h"

TEST_CASE("ModelLoader") {
	Model* cube = ModelLoader::loadModel("../Game/assets/cube.obj");
	REQUIRE(cube != nullptr);

	SECTION("Does not allow duplicated memory of models") {
		Model* duplicateAttempt = ModelLoader::loadModel("../Game/assets/cube.obj");

		REQUIRE(cube == duplicateAttempt);
	}

	SECTION("Creates mesh for models") {
		REQUIRE(cube->meshCount() > 0);

		SECTION("Attributes a material index to each mesh") {
			for (unsigned int i = 0; i < cube->meshCount(); i += 1) {
				unsigned short materialIndex = cube->getMesh(i)->getMaterialIndex();

				CHECK(materialIndex < cube->materialCount());
			}
		}
	}

	SECTION("Returns nullptr when loading non-existent files") {
		CHECK(!ModelLoader::loadModel("non-existent"));
	}

	SECTION("Is able to unload models from memory") {
		ModelLoader::unloadModels();

		CHECK(ModelLoader::modelCount() == 0);
	}
}

TEST_CASE("TextureManager") {
	Texture* texture = TextureManager::loadTexture("../Game/assets/stone_wall_diffuse.jpg", TXTYPE_DIFFUSE);

	REQUIRE(texture != nullptr);

	SECTION("Only loads the desired texture type") {
		// Require that the desired type is attributed
		REQUIRE(texture->type == TXTYPE_DIFFUSE);

		// Check that a different type cannot be attributed to an already loaded texture
		Texture* textureCopy = TextureManager::loadTexture("../Game/assets/stone_wall_diffuse.jpg", TXTYPE_SPECULAR);

		CHECK(textureCopy->type != TXTYPE_SPECULAR);
	}

	SECTION("Returns nullptr when loading non-existent files") {
		CHECK(!TextureManager::loadTexture("non-existent", TXTYPE_SPECULAR));
	}

	SECTION("Is able to unload textures from memory") {
		TextureManager::unloadTextures();
		CHECK(TextureManager::textureCount() == 0);
	}
}