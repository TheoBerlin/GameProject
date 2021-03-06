#include "ScoreManager.h"

#include <Engine/Events/EventBus.h>
#include "Engine/GUI/Panel.h"
#include "Engine/GUI/Button.h"
#include "Engine/GUI/GUI.h"
#include "Game/Level/Level.h"
#include "Utils/Timer.h"
#include "Utils/Logger.h"

ScoreManager::ScoreManager()
{
	this->totalTime = 0.0f;
	this->totalScore = 0;
	this->optimalTime = 0.0f;
	this->timer = new Timer();
	this->newHighscore = false;
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

void ScoreManager::setHighscore(const unsigned highscore)
{
	this->highscore = highscore;
}

int ScoreManager::getHighscore() const
{
	return this->highscore;
}

float ScoreManager::getOptimalTime() const
{
	return optimalTime;
}

float ScoreManager::getTotalTime() const
{
	return this->totalTime;
}

unsigned ScoreManager::getTargetsHit() const
{
	return this->targetsHit;
}

void ScoreManager::resetScore()
{
	this->totalScore = 0;
	this->targetsHit = 0;
}

void ScoreManager::showResults(Level& level, const std::function<void()>& retry)
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
	smallPanel->setOption(GUI::FLOAT_LEFT);
	smallPanel->setOption(GUI::FLOAT_UP);
	smallPanel->setOption(GUI::SCALE_TO_TEXT_X, 20);
	smallPanel->setOption(GUI::SCALE_TO_TEXT_Y, 20);
	smallPanel->setOption(GUI::TEXT_CENTER_X);
	smallPanel->setOption(GUI::TEXT_CENTER_Y);
	smallPanel->addText("MENU", "aldo", textColor);
	smallPanel->setNormalColor(backgroundColor);
	smallPanel->setHoverColor(hoverColor);
	smallPanel->setPressedColor(pressColor);
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
	std::function<void(void)> callback = std::bind(&ScoreManager::toggleGuiMinimize, this);
	miniBtn->setCallback(callback);
	smallPanel->setCallback(callback);
	bigPanel->addChild(miniBtn);

	if (level.targetManager->getTargetCount() == this->targetsHit) {
		if (this->totalScore > this->highscore) {
			this->highscore = this->totalScore;
			EventBus::get().publish(&NewHighscoreEvent(highscore));
			this->newHighscore = true;
		}

		// Add text - Title
		bigPanel->addText("SCORE", "aldo", textColor);
		unsigned width = bigPanel->getText(0)->getWidth();
		bigPanel->updateText(0, panelSize.x / 2 - width / 2, panelSize.y - 60);

		// Add text - Score
		bigPanel->addText("SCORE: " + std::to_string(level.scoreManager->getScore()), "aldo", textColor);
		bigPanel->updateText(1, 20, panelSize.y - 140);


		// Add text - Highscore
		bigPanel->addText("HIGHSCORE: " + std::to_string(level.scoreManager->getHighscore()), "aldo", textColor);
		bigPanel->updateText(2, 20, panelSize.y - 200);

		// Add text - Targets
		std::string t = "TARGETS: " + std::to_string(level.scoreManager->getTargetsHit()) + "/" + std::to_string(level.targetManager->getTargetCount());
		bigPanel->addText(t, "aldo", textColor);
		bigPanel->updateText(3, 20, panelSize.y - 260);

		// Add text - "Not all targets hit"
		bigPanel->addText("LEVEL COMPLETE", "aldo", textColor);
		width = bigPanel->getText(4)->getWidth();
		bigPanel->updateText(4, panelSize.x / 2 - width / 2, panelSize.y - 320);

		if (this->newHighscore) {
			// Add text - "New Highscore"
			bigPanel->addText("NEW HIGHSCORE", "aldo", textColor);
			width = bigPanel->getText(5)->getWidth();
			bigPanel->updateText(5, panelSize.x / 2 - width / 2, panelSize.y - 380);
			this->newHighscore = false;
		}

	}
	else {
		// Add text - Title
		bigPanel->addText("SCORE", "aldo", textColor);
		unsigned width = bigPanel->getText(0)->getWidth();
		bigPanel->updateText(0, panelSize.x / 2 - width / 2, panelSize.y - 60);

		// Add text - Score
		bigPanel->addText("SCORE: " + std::to_string(0), "aldo", textColor);
		bigPanel->updateText(1, 20, panelSize.y - 140);

		// Add text - Highscore
		bigPanel->addText("HIGHSCORE: " + std::to_string(level.scoreManager->getHighscore()), "aldo", textColor);
		bigPanel->updateText(2, 20, panelSize.y - 200);

		// Add text - Targets
		std::string t = "TARGETS: " + std::to_string(level.scoreManager->getTargetsHit()) + "/" + std::to_string(level.targetManager->getTargetCount());
		bigPanel->addText(t, "aldo", textColor);
		bigPanel->updateText(3, 20, panelSize.y - 260);

		// Add text - "Not all targets hit"
		bigPanel->addText("NOT ALL TARGETS HIT", "aldo", textColor);
		width = bigPanel->getText(4)->getWidth();
		bigPanel->updateText(4, panelSize.x / 2 - width / 2, panelSize.y - 320);

		// Add text - "Try again"
		bigPanel->addText("TRY AGAIN", "aldo", textColor);
		width = bigPanel->getText(5)->getWidth();
		bigPanel->updateText(5, panelSize.x / 2 - width / 2, panelSize.y - 380);
	}

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
	exitBtn->setCallback([](void) {
		EventBus::get().publish(&ExitEvent());
	});
	bigPanel->addChild(exitBtn);

	// Add retry button
	Button* retryBtn = new Button();
	retryBtn->setSize({ panelSize.x - 40, 50 });
	retryBtn->addText("RETRY", "aldo", textColor);
	retryBtn->setOption(GUI::FLOAT_DOWN, 80);
	retryBtn->setOption(GUI::CENTER_X);
	retryBtn->setOption(GUI::TEXT_CENTER_X);
	retryBtn->setOption(GUI::TEXT_CENTER_Y);
	retryBtn->setHoverColor(hoverColor);
	retryBtn->setPressedColor(pressColor);
	retryBtn->setCallback(retry);
	bigPanel->addChild(retryBtn);

	// Set initial state
	this->minimized = true;
	toggleGuiMinimize();
	this->showGui = true;
}

bool ScoreManager::resultsVisible() const
{
	return this->showGui;
}

void ScoreManager::removeResultsGUI(Level& level)
{
	level.gui->removePanel(this->bigPanel);
	level.gui->removePanel(this->smallPanel);

	this->bigPanel = nullptr;
	this->smallPanel = nullptr;

	this->showGui = false;
}

void ScoreManager::toggleGuiMinimize()
{
	int width = Display::get().getWidth();
	if (this->minimized)
	{
		// Move smallPanel
		this->smallPanel->setOption(GUI::FLOAT_LEFT, -width);
		this->smallPanel->setActive(false);

		// Move bigPanel
		this->bigPanel->setActive(true);
		this->bigPanel->setOption(GUI::CENTER_X, 0);

		this->minimized = false;
	}
	else
	{
		// Move bigPanel
		this->bigPanel->setOption(GUI::CENTER_X, -width);
		this->bigPanel->setActive(false);


		// Move smallPanel
		this->smallPanel->setActive(true);
		this->smallPanel->setOption(GUI::FLOAT_LEFT, 0);


		this->minimized = true;
	}
}