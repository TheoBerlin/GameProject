#pragma once

#include "Engine/Events/Events.h"
#include "glm/glm.hpp"

#include <string>

enum PHASE
{
	OVERVIEW = 0,
	AIM,
	GUIDING,
	REPLAY
};

class GUI;
class Panel;

class HelpGUI
{
public:
	HelpGUI();
	~HelpGUI();

	void init(GUI* gui);

	void switchPhase(PHASE phase);

	void update(const float& dt);

private:
	float totalTime;
	const unsigned maxMove = 200;
	const glm::uvec2 innerPanelSize = { 200, 50 };
	const float animateTime = 0.5f;
	bool in;

	GUI* gui;

	Panel* currentGUI;
	Panel* overviewGUI;
	Panel* aimGUI;
	Panel* guidingGUI;
	Panel* replayGUI;

	void switchGUI(Panel* newGUI);

	void createOverviewGUI();
	void createAimGUI();
	void createGuidingGUI();
	void createReplayGUI();

	void addButton(Panel* parent, std::string text, std::string icon);

	void animate(const float& dt, bool in);
	bool moveIn;
	bool moveOut;

	void toggleGUI(KeyEvent* e);
};

