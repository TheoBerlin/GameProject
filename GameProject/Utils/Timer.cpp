#include "Timer.h"

Timer::Timer()
{
	this->isRunning = true;
	restart();
}

Timer::~Timer()
{
}

void Timer::start()
{
	this->isRunning = true;
	this->startTime = std::chrono::high_resolution_clock::now();
	this->currTime = this->startTime;
	this->preTime = this->startTime;
	this->dt = 0.0f;
}

void Timer::stop()
{
	this->isRunning = false;
	this->currTime = std::chrono::high_resolution_clock::now();
	this->dt = std::chrono::duration<float>(this->currTime - this->startTime).count();
	this->preTime = this->currTime;
	this->dt = 0.0f;
}

void Timer::restart()
{
	this->isRunning = true;
	this->startTime = std::chrono::high_resolution_clock::now();
	this->currTime = this->startTime;
}

void Timer::update()
{
	if (this->isRunning)
	{
		this->currTime = std::chrono::high_resolution_clock::now();
		this->dt = std::chrono::duration<float>(this->currTime - this->preTime).count();
		this->preTime = this->currTime;
	}
}

float Timer::getTime()
{
	return std::chrono::duration<float>(this->currTime - this->startTime).count();
}

float Timer::getDeltaTime() const
{
	return this->dt;
}