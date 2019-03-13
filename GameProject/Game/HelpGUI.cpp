#include "HelpGUI.h"

#include "Engine/Events/EventBus.h"
#include "Engine/GUI/GUI.h"
#include "Engine/GUI/GUIColors.h"
#include "Engine/GUI/Panel.h"
#include "Engine/AssetManagement/TextureManager.h"

HelpGUI::HelpGUI()
{
	this->currentGUI = nullptr;
	this->moveIn = false;
	this->moveOut = false;
	EventBus::get().subscribe(this, &HelpGUI::toggleGUI);
	this->in = true;
}


HelpGUI::~HelpGUI()
{
	EventBus::get().unsubscribe(this, &HelpGUI::toggleGUI);
}

void HelpGUI::init(GUI* gui)
{
	this->gui = gui;

	createOverviewGUI();
	createAimGUI();
	createGuidingGUI();
	createReplayGUI();

	switchPhase(PHASE::OVERVIEW);
}

void HelpGUI::switchPhase(PHASE phase)
{
	switch (phase)
	{
	case PHASE::OVERVIEW:
		switchGUI(this->overviewGUI);
		break;
	case PHASE::AIM:
		switchGUI(this->aimGUI);
		break;
	case PHASE::GUIDING:
		switchGUI(this->guidingGUI);
		break;
	case PHASE::REPLAY:
		switchGUI(this->replayGUI);
		break;
	}
}

void HelpGUI::update(const float & dt)
{
	if (moveIn)
		animate(dt, true);
	else if (moveOut)
		animate(dt, false);
}

void HelpGUI::switchGUI(Panel* newGUI)
{
	if (this->currentGUI)
		this->currentGUI->hide();
	this->currentGUI = newGUI;
	this->currentGUI->show();
}

void HelpGUI::createOverviewGUI()
{
	// Container panel for overview
	this->overviewGUI = new Panel();
	this->overviewGUI->setOption(GUI::FLOAT_LEFT);
	this->overviewGUI->setSize({ 500, 0 });
	this->overviewGUI->setOption(GUI::FIT_Y);
	this->overviewGUI->setColor({ 1.f, 1.f, 1.f, 0.f });
	this->gui->addPanel(this->overviewGUI);

	// Add "Press 2 for guiding" icon
	Panel* guidPnl = new Panel();
	guidPnl->setSize({ 50, 50 });
	guidPnl->setOption(GUI::FLOAT_LEFT, 200);
	guidPnl->setOption(GUI::FLOAT_UP, 200);
	guidPnl->setColor({1.f, 1.f, 1.f, 1.f});
	guidPnl->setBackgroundTexture(TextureManager::loadTexture("./Game/Assets/buttons/2.png"));
	this->overviewGUI->addChild(guidPnl);

	// Add "Guiding" text
	Panel* guidPnlText = new Panel();
	guidPnlText->setSize({ this->innerPanelSize });
	guidPnlText->setOption(GUI::FLOAT_LEFT);
	guidPnlText->setOption(GUI::FLOAT_UP, 200);
	guidPnlText->setOption(GUI::TEXT_FLOAT_LEFT, 10);
	guidPnlText->setOption(GUI::TEXT_CENTER_Y);
	guidPnlText->setColor(PANEL_BACKGROUND_COLOR_TRANSPARENT);
	guidPnlText->addText("Guiding", "aldo", TEXT_COLOR);
	this->overviewGUI->addChild(guidPnlText);

	// Add "WASD for control"
	Panel* wasdPnl = new Panel();
	wasdPnl->setSize({ 50, 50 });
	wasdPnl->setOption(GUI::FLOAT_LEFT, 200);
	wasdPnl->setOption(GUI::FLOAT_UP, 250);
	wasdPnl->setOption(GUI::TEXT_CENTER_X);
	wasdPnl->setOption(GUI::TEXT_CENTER_Y);
	wasdPnl->setColor({ 1.f, 1.f, 1.f, 1.f });
	wasdPnl->setBackgroundTexture(TextureManager::loadTexture("./Game/Assets/buttons/W.png"));
	this->overviewGUI->addChild(wasdPnl);

	// Add "Camera" text
	Panel* wasdPnlText = new Panel();
	wasdPnlText->setSize({ this->innerPanelSize });
	wasdPnlText->setOption(GUI::FLOAT_LEFT);
	wasdPnlText->setOption(GUI::FLOAT_UP, 250);
	wasdPnlText->setOption(GUI::TEXT_FLOAT_LEFT, 10);
	wasdPnlText->setOption(GUI::TEXT_CENTER_Y);
	wasdPnlText->setColor(PANEL_BACKGROUND_COLOR_TRANSPARENT);
	wasdPnlText->addText("Camera", "aldo", TEXT_COLOR);
	this->overviewGUI->addChild(wasdPnlText);

	// Add "Ctrl to lower"
	Panel* ctrlPnl = new Panel();
	ctrlPnl->setSize({ 100, 50 });
	ctrlPnl->setOption(GUI::FLOAT_LEFT, 200);
	ctrlPnl->setOption(GUI::FLOAT_UP, 300);
	ctrlPnl->setOption(GUI::TEXT_CENTER_X);
	ctrlPnl->setOption(GUI::TEXT_CENTER_Y);
	ctrlPnl->setColor({ 1.f, 1.f, 1.f, 1.f });
	ctrlPnl->setBackgroundTexture(TextureManager::loadTexture("./Game/Assets/buttons/ctrl.png"));
	this->overviewGUI->addChild(ctrlPnl);

	// Add "Decend" text
	Panel* ctrlPnlText = new Panel();
	ctrlPnlText->setSize({ this->innerPanelSize });
	ctrlPnlText->setOption(GUI::FLOAT_LEFT);
	ctrlPnlText->setOption(GUI::FLOAT_UP, 300);
	ctrlPnlText->setOption(GUI::TEXT_FLOAT_LEFT, 10);
	ctrlPnlText->setOption(GUI::TEXT_CENTER_Y);
	ctrlPnlText->setColor(PANEL_BACKGROUND_COLOR_TRANSPARENT);
	ctrlPnlText->addText("Descend", "aldo", TEXT_COLOR);
	this->overviewGUI->addChild(ctrlPnlText);

	// Add "Shift to higher"
	Panel* shiftPnl = new Panel();
	shiftPnl->setSize({ 100, 50 });
	shiftPnl->setOption(GUI::FLOAT_LEFT, 200);
	shiftPnl->setOption(GUI::FLOAT_UP, 350);
	shiftPnl->setColor({ 1.f, 1.f, 1.f, 1.f });
	shiftPnl->setBackgroundTexture(TextureManager::loadTexture("./Game/Assets/buttons/shift.png"));
	this->overviewGUI->addChild(shiftPnl);

	// Add "Ascend" text
	Panel* shiftPnlText = new Panel();
	shiftPnlText->setSize({ this->innerPanelSize });
	shiftPnlText->setOption(GUI::FLOAT_LEFT);
	shiftPnlText->setOption(GUI::FLOAT_UP, 350);
	shiftPnlText->setOption(GUI::TEXT_FLOAT_LEFT, 10);
	shiftPnlText->setOption(GUI::TEXT_CENTER_Y);
	shiftPnlText->setColor(PANEL_BACKGROUND_COLOR_TRANSPARENT);
	shiftPnlText->addText("Ascend", "aldo", TEXT_COLOR);
	this->overviewGUI->addChild(shiftPnlText);
}

void HelpGUI::createAimGUI()
{
}

void HelpGUI::createGuidingGUI()
{
}

void HelpGUI::createReplayGUI()
{
}

void HelpGUI::addButton(Panel * parent, std::string text, std::string icon)
{
	Panel* p = new Panel();
}

void HelpGUI::animate(const float& dt, bool in)
{
	if (this->totalTime < this->animateTime)
	{
		this->totalTime += dt;

		float progress = this->totalTime / this->animateTime;

		if (!in)
			progress = (1.f - progress);

		this->currentGUI->setOption(GUI::FLOAT_LEFT, (int)((this->maxMove * progress) - this->maxMove));
	}
}

void HelpGUI::toggleGUI(KeyEvent * e)
{
	if (e->action == GLFW_PRESS && e->key == GLFW_KEY_F1)
	{
		if (in)
		{
			this->totalTime = 0;
			moveOut = true;
			moveIn = false;
			in = !in;
		}
		else
		{
			this->totalTime = 0;
			moveIn = true;
			moveOut = false;
			in = !in;
		}
	}
}
