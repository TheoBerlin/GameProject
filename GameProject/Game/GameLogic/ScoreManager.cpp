#include "ScoreManager.h"

#include "Utils/Timer.h"
#include "Engine/GUI/Panel.h"
#include "Engine/GUI/Button.h"
#include "Engine/GUI/GUI.h"
#include "Game/Level/Level.h"

#include "Utils/Logger.h"

ScoreManager::ScoreManager()
{
	this->totalTime = 0.0f;
	this->totalScore = 0;
	this->optimalTime = 0.0f;
	this->timer = new Timer();
}


ScoreManager::~ScoreManager()
{
	delete this->timer;
}

void ScoreManager::start()
{
	this->counting = true;
	this->totalScore = 0;
	this->totalTime = 0.0f;
	this->timer->restart();
}

void ScoreManager::stop()
{
	this->timer->stop();
	this->totalTime = this->timer->getTime();
	this->counting = false;

	// Calculate the new total score with time as a factor
	float inv = (1.0f / this->totalTime) * 100;
	LOG_INFO("Time: %f Inverse time: %f Optimal time: %f", this->totalTime, inv, this->optimalTime);
	this->totalScore += (unsigned)inv;
}

unsigned ScoreManager::getScore() const
{
	return this->totalScore;
}

void ScoreManager::score()
{
	this->totalScore += 10;
	this->targetsHit++;
}

void ScoreManager::scoreBonus()
{
	this->totalScore += 20;
	this->targetsHit++;
}

void ScoreManager::setOptimalTime(const float & time)
{
	this->optimalTime = time;
}

float ScoreManager::getTotalTime() const
{
	return this->totalTime;
}

unsigned ScoreManager::getTargetsHit() const
{
	return this->targetsHit;
}

void ScoreManager::showResults(Level& level)
{
	glm::uvec2 panelSize(500, 700);
	glm::vec4 textColor = { 0.9f, 0.9f, 0.9f, 1.0f };
	glm::vec4 backgroundColor = { 0.1f, 0.1f, 0.1f, 0.99f };
	glm::vec4 hoverColor = { 0.5f, 0.0f, 0.5f, 1.0f };
	glm::vec4 pressColor = { 0.3f, 0.0f, 0.3f, 1.0f };

	// Create outer panel
	Panel* bigPanel = new Panel();
	bigPanel->setSize(panelSize);
	bigPanel->setOption(GUI::CENTER_X);
	bigPanel->setOption(GUI::CENTER_Y);
	bigPanel->setColor(backgroundColor);
	this->bigPanel = bigPanel;
	level.gui->addPanel(bigPanel);

	// Create minimized panel/button
	Button* smallPanel = new Button();
	smallPanel->setSize({ 70, 50 });
	smallPanel->setOption(GUI::FLOAT_LEFT);
	smallPanel->setOption(GUI::FLOAT_UP);
	smallPanel->addText("Show", "aldo", textColor);
	smallPanel->setColor(backgroundColor);
	this->smallPanel = smallPanel;
	level.gui->addPanel(smallPanel);

	// Add minimize button
	Button* miniBtn = new Button();
	miniBtn->setSize({ panelSize.x - 40, 50 });
	miniBtn->setOption(GUI::FLOAT_DOWN, 140);
	miniBtn->setOption(GUI::CENTER_X);
	miniBtn->setOption(GUI::TEXT_CENTER_X);
	miniBtn->setOption(GUI::TEXT_CENTER_Y);
	miniBtn->setHoverColor(hoverColor);
	miniBtn->setPressedColor(pressColor);
	miniBtn->addText("MINIMIZE", "aldo", textColor);

	// assign callback function
	std::function<void(void)> callback = std::bind(&ScoreManager::guiCallback, this);
	miniBtn->setCallback(callback);
	smallPanel->setCallback(callback);
	bigPanel->addChild(miniBtn);

	// Add text - Title
	bigPanel->addText("Score", "aldo", textColor);
	unsigned width = bigPanel->getText(0)->getWidth();
	bigPanel->updateText(0, panelSize.x / 2 - width / 2, panelSize.y - 60);

	// Add text - Score
	bigPanel->addText("Score: " + std::to_string(level.scoreManager->getScore()), "aldo", textColor);
	bigPanel->updateText(1, 20, panelSize.y - 140);

	// Add text - Targets
	std::string t = "Targets: " + std::to_string(level.scoreManager->getTargetsHit()) + "/" + std::to_string(level.targetManager->getTargetCount());
	bigPanel->addText(t, "aldo", textColor);
	bigPanel->updateText(2, 20, panelSize.y - 200);

	// Add exit button
	Button* exitBtn = new Button();
	exitBtn->setSize({ panelSize.x - 40, 50 });
	exitBtn->addText("EXIT", "aldo", textColor);
	exitBtn->setOption(GUI::FLOAT_DOWN, 20);
	exitBtn->setOption(GUI::CENTER_X);
	exitBtn->setOption(GUI::TEXT_CENTER_X);
	exitBtn->setOption(GUI::TEXT_CENTER_Y);
	exitBtn->setHoverColor(hoverColor);
	exitBtn->setPressedColor(pressColor);
	exitBtn->setCallback([](void) { printf("add exit\n"); });
	bigPanel->addChild(exitBtn);

	// Add restart button
	Button* resBtn = new Button();
	resBtn->setSize({ panelSize.x - 40, 50 });
	resBtn->addText("RESTART", "aldo", textColor);
	resBtn->setOption(GUI::FLOAT_DOWN, 80);
	resBtn->setOption(GUI::CENTER_X);
	resBtn->setOption(GUI::TEXT_CENTER_X);
	resBtn->setOption(GUI::TEXT_CENTER_Y);
	resBtn->setHoverColor(hoverColor);
	resBtn->setPressedColor(pressColor);
	resBtn->setCallback([](void) { printf("add restart\n"); });
	bigPanel->addChild(resBtn);


	// Set initial state
	this->minimized = true;
	guiCallback();
	this->showGui = true;
}

bool ScoreManager::resultsVisible() const
{
	return this->showGui;
}

void ScoreManager::guiCallback()
{
	int width = Display::get().getWidth();
	if (this->minimized)
	{
		// Move smallPanel
		this->smallPanel->setOption(GUI::FLOAT_LEFT, -width);

		// Move bigPanel
		this->bigPanel->setOption(GUI::CENTER_X, 0);

		this->minimized = false;
	}
	else
	{
		// Move bigPanel
		this->bigPanel->setOption(GUI::CENTER_X, -width);

		// Move smallPanel
		this->smallPanel->setOption(GUI::FLOAT_LEFT, 0);

		this->minimized = true;
	}
}
