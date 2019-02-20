#pragma once

#include "../Engine/IGame.h"

#include "States/MenuState.h"

class Game : public IGame
{
public:
	Game();
	virtual ~Game();

	void onUpdate(float dt) override;
	void onUpdateLogic(const float dt) override;
	void onRender() override;
};