#include "GuidingPhase.h"

#include <Engine/AssetManagement/ModelLoader.h>
#include <Engine/Components/Camera.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Engine/Components/CollisionComponent.h>
#include <Game/Components/PathVisualizer.h>
#include <Game/GameLogic/AimPhase.h>
#include <Game/GameLogic/OverviewPhase.h>
#include <Game/GameLogic/ReplayPhase.h>
#include <Utils/Logger.h>

GuidingPhase::GuidingPhase(AimPhase* aimPhase)
    :Phase((Phase*)aimPhase)
{
	this->playerArrow = aimPhase->getPlayerArrow();

    // Start guiding the arrow
    arrowGuider = aimPhase->getArrowGuider();
    arrowGuider->startGuiding();

    level.targetManager->resetTargets();

	/*
	Do stuff when collision happens
	*/
	EventBus::get().subscribe(this, &GuidingPhase::playerCollisionCallback);

	level.collisionHandler->addCollisionToEntity(this->playerArrow, SHAPE::ARROW);
}

Entity* GuidingPhase::getPlayerArrow() const
{
    return playerArrow;
}

ArrowGuider* GuidingPhase::getArrowGuider() const
{
    return arrowGuider;
}

void GuidingPhase::handleKeyInput(KeyEvent* event)
{

    if (event->action != GLFW_PRESS) {
        return;
    }

    if (event->key == GLFW_KEY_3) {
		EventBus::get().unsubscribe(this, &GuidingPhase::playerCollisionCallback);

		level.collisionHandler->removeCollisionBody(this->playerArrow);

        Phase* replayPhase = new ReplayPhase(this);
        changePhase(replayPhase);
    }
}


void GuidingPhase::playerCollisionCallback(PlayerCollisionEvent * ev)
{
	// Entity1 should always be player, but to be on the safe side...
	Entity* otherEntity;
	const rp3d::ProxyShape* playerShape;
	const rp3d::ProxyShape* otherShape;
	if (ev->entity1 != this->playerArrow)
	{
		otherEntity = ev->entity1;
		otherShape = ev->shape1;
		playerShape = ev->shape2;
	}
	else
	{
		otherEntity = ev->entity2;
		otherShape = ev->shape2;
		playerShape = ev->shape1;
	}

	// Handle collision for the entity the arrow hit
	CollisionComponent* collision = dynamic_cast<CollisionComponent*>(otherEntity->getComponent("Collision"));
	if (collision != nullptr)
		collision->handleCollision(otherShape, playerShape);

	// Only check if there is a player assigned
	if (this->playerArrow) {
		// Handle collision for the player arrow
		CollisionComponent* playerCollision = dynamic_cast<CollisionComponent*>(this->playerArrow->getComponent("Collision"));
		if (playerCollision != nullptr)
		{
			// 
			unsigned category = otherShape->getCollisionCategoryBits();
			switch (category)
			{
			case CATEGORY::DRONE_BODY:
			{
				// Score point
				break;
			}
			case CATEGORY::DRONE_EYE:
			{
				// Score bonus points
				break;
			}
			case CATEGORY::STATIC:
			{
				// Arrow hit a static object - destory arrow
				break;
			}
			default:
				break;
			}
			playerCollision->handleCollision(playerShape, otherShape);
		}
	}
}
