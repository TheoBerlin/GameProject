#include "ScoreManager.h"

#include "Utils/Timer.h"

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
