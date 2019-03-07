#pragma once

#include <Engine/Events/Events.h>

/// Forward declarations
class Entity;
namespace reactphysics3d { class ProxyShape; }

struct PlayerCollisionEvent : public Event
{
	PlayerCollisionEvent(Entity* entity1, Entity* entity2, const reactphysics3d::ProxyShape * shape1, const reactphysics3d::ProxyShape * shape2) : entity1{ entity1 }, entity2{ entity2 }, shape1{ shape1 }, shape2{ shape2 } {};
	Entity* entity1;
	Entity* entity2;
	const reactphysics3d::ProxyShape* shape1;
	const reactphysics3d::ProxyShape* shape2;
};

class Phase;

struct PhaseChangeEvent : public Event
{
	PhaseChangeEvent(Phase* newPhase) : newPhase{ newPhase } {};
	Phase* newPhase;
};

class Entity;

struct CameraTransitionEvent : public Event
{
	CameraTransitionEvent(Entity* host) : host{ host } {};
	Entity* host;
};
