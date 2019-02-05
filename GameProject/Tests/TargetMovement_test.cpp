#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Catch/catch.hpp>

#include "Engine/Components/Component.h"
#include "Engine/Components/TargetMovementComponent.h"
#include "Engine/Entity/Entity.h"
#include "glm/gtx/string_cast.hpp"
#include <iostream>

TEST_CASE("TargetMovemetTest") {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Entity entity;
	entity.setName("Target1");

	REQUIRE(entity.getName() == "Target1");
	
	TargetMovementComponent* targets = new TargetMovementComponent(&entity, "MovingTarget1");

	REQUIRE(targets->getName() == "MovingTarget1");

	SECTION("Add a position to the targets") {
		entity.getMatrix()->translate(glm::vec3(1.0f, 1.0f, 1.0f));
		glm::vec3 aPos = targets->addPosition(entity.getMatrix()->getPosition());

		REQUIRE(aPos.x == 6.0f);
	}


}


