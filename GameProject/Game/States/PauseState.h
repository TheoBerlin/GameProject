#pragma once

#include "../../Engine/States/State.h"
#include "Engine/Rendering/GLAbstraction/Texture.h"

class KeyEvent;

class PauseState : public State
{
public:
	PauseState();
	~PauseState();

	void start() override;
	void end() override;
	void update(const float dt) override;
	void updateLogic(const float dt) override;
	void render() override;

private:
	void initPanelLayout();
	void unpauseGame(KeyEvent * ev);

	bool hasSubscribed;

	Texture backgroundTexture;
};

