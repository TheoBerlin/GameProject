#include "GuidingPhase.h"

#include <Engine/AssetManagement/ModelLoader.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Engine/Components/CollisionComponent.h>
#include <Game/Components/PathVisualizer.h>
#include <Game/GameLogic/AimPhase.h>
#include <Game/GameLogic/ReplayPhase.h>

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

    EventBus::get().subscribe(this, &GuidingPhase::handleKeyInput);
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
        EventBus::get().unsubscribe(this, &GuidingPhase::handleKeyInput);

		// Get flight time
		flightTimer.stop();
		float flightTime = flightTimer.getDeltaTime();

        arrowGuider->stopGuiding(flightTime);

        // Begin camera transition to the replay freecam
        glm::vec3 newPos = level.player.replayCamera.position;
        glm::vec3 newForward = level.player.replayCamera.direction;
        float transitionLength = 2.0f;

        glm::vec3 currentPosition = playerArrow->getTransform()->getPosition();
        glm::vec3 currentForward = playerArrow->getTransform()->getForward();

        transitionEntity->getTransform()->setPosition(currentPosition);
        transitionEntity->getTransform()->setForward(currentForward);

        transitionComponent->setDestination(newPos, newForward, transitionLength);

        Display::get().getRenderer().setActiveCamera(transitionCam);

        EventBus::get().subscribe(this, &GuidingPhase::transitionToReplay);

        transitionComponent->setDestination(newPos, newForward, transitionLength);
    }
}

void GuidingPhase::transitionToReplay(CameraTransitionEvent* event)
{
    EventBus::get().unsubscribe(this, &GuidingPhase::transitionToReplay);

	level.collisionHandler->removeCollisionBody(this->playerArrow);
	EventBus::get().unsubscribe(this, &GuidingPhase::playerCollisionCallback);

    Phase* guidingPhase = new ReplayPhase(this);
    changePhase(guidingPhase);
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
