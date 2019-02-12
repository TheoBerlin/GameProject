#include "CollisionHandler.h"


CollisionHandler::CollisionHandler()
{
	this->test = new rp3d::CollisionWorld();
}


CollisionHandler::~CollisionHandler()
{
	delete test;
}
