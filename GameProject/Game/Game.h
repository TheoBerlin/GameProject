#pragma once

#include "../Engine/IGame.h"

class Game : public IGame
{
public:
	Game();
	virtual ~Game();

	void onUpdate(float dt) override;
	void onUpdateLogic() override;
	void onRender() override;

private:

};