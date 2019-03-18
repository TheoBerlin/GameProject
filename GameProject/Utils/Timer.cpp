#include "Timer.h"

Timer::Timer()
{
	this->dt = 0.f;
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
}

double Timer::restart()
{
	double elapsedTime = this->getTime();

	this->dt = 0;
	this->isRunning = true;
	this->startTime = std::chrono::high_resolution_clock::now();
	this->currTime = this->startTime;

	return elapsedTime;
}

void Timer::update()
{
	if (this->isRunning)
	{
		this->currTime = std::chrono::high_resolution_clock::now();
		using sec = std::chrono::duration<float>;
		this->dt = std::chrono::duration_cast<sec>(this->currTime - this->startTime).count();
		this->preTime = this->currTime;
	}
}

float Timer::getTime()
{
	using sec = std::chrono::duration<float>;
	return std::chrono::duration_cast<sec>(this->currTime - this->startTime).count();
}

float Timer::getDeltaTime() const
{
	return this->dt;
}