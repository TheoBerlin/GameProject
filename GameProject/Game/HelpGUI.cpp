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

	if (in)
		this->currentGUI->setOption(GUI::FLOAT_LEFT, 0);
	else
		this->currentGUI->setOption(GUI::FLOAT_LEFT, -(int)this->maxMove);

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

	addButton(this->overviewGUI, "AIM", "2.png");
	addButton(this->overviewGUI, "CAMERA", "wasdMini.png");
	addButton(this->overviewGUI, "DESCEND", "ctrl.png");
	addButton(this->overviewGUI, "ASCEND", "shift.png");

	this->overviewGUI->hide();
}

void HelpGUI::createAimGUI()
{
	// Container panel for aim
	this->aimGUI = new Panel();
	this->aimGUI->setOption(GUI::FLOAT_LEFT);
	this->aimGUI->setSize({ 500, 0 });
	this->aimGUI->setOption(GUI::FIT_Y);
	this->aimGUI->setColor({ 1.f, 1.f, 1.f, 0.f });
	this->gui->addPanel(this->aimGUI);

	addButton(this->aimGUI, "OVERVIEW", "1.png");
	addButton(this->aimGUI, "SHOOT", "mouse1.png");

	this->aimGUI->hide();
}

void HelpGUI::createGuidingGUI()
{
	// Container panel for guiding
	this->guidingGUI = new Panel();
	this->guidingGUI->setOption(GUI::FLOAT_LEFT);
	this->guidingGUI->setSize({ 500, 0 });
	this->guidingGUI->setOption(GUI::FIT_Y);
	this->guidingGUI->setColor({ 1.f, 1.f, 1.f, 0.f });
	this->gui->addPanel(this->guidingGUI);

	addButton(this->guidingGUI, "REPLAY", "3.png");
	
	this->guidingGUI->hide();
}

void HelpGUI::createReplayGUI()
{
	// Container panel for replay
	this->replayGUI = new Panel();
	this->replayGUI->setOption(GUI::FLOAT_LEFT);
	this->replayGUI->setSize({ 500, 0 });
	this->replayGUI->setOption(GUI::FIT_Y);
	this->replayGUI->setColor({ 1.f, 1.f, 1.f, 0.f });
	this->gui->addPanel(this->replayGUI);

	addButton(this->replayGUI, "AIM", "2.png");
	addButton(this->replayGUI, "DETATCH", "space.png");

	this->replayGUI->hide();
}

void HelpGUI::addButton(Panel * parent, std::string text, std::string icon)
{
	unsigned offset = this->heightOffset + 50 * (parent->getChildren().size() / 2);

	// Create icon panel
	Panel* p = new Panel();
	p->setOption(GUI::SCALE_TEXTURE_TO_HEIGHT, 50);
	p->setOption(GUI::FLOAT_LEFT, this->innerPanelSize.x);
	p->setOption(GUI::FLOAT_UP, offset);
	p->setColor({ 1.f, 1.f, 1.f, 1.f });
	p->setBackgroundTexture(TextureManager::loadTexture("./Game/Assets/buttons/" + icon));
	parent->addChild(p);

	// Create text panel
	Panel* t = new Panel();
	t->setSize(this->innerPanelSize);
	t->setOption(GUI::FLOAT_LEFT);
	t->setOption(GUI::FLOAT_UP, offset);
	t->setOption(GUI::TEXT_FLOAT_LEFT, 10);
	t->setOption(GUI::TEXT_CENTER_Y);
	t->setColor(PANEL_BACKGROUND_COLOR_TRANSPARENT);
	t->addText(text, "aldo", TEXT_COLOR);
	parent->addChild(t);
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
