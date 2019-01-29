#pragma once

#include <chrono>

class Timer
{
public:
	// Will start the timer when created.
	Timer();
	virtual ~Timer();

	// Start the timer.
	void start();

	// Stop the timer.
	void stop();

	// Restart the timer.
	void restart();

	// Update delta time.
	void update();

	// The time from starting/restarting the timer.
	float getTime();

	// The time from previous update.
	float getDeltaTime() const;

private:
	float dt;
	std::chrono::time_point<std::chrono::steady_clock> startTime;
	std::chrono::time_point<std::chrono::steady_clock> preTime;
	std::chrono::time_point<std::chrono::steady_clock> currTime;
	bool isRunning;
};