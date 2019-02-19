#pragma once

#include <vector>

#include "Timer.h"

class TimeProfiler
{
public:
	/*
		title of time profiler and indicate if IMGUI should display statistic
	*/
	TimeProfiler(const std::string & title = "", bool displayGUI = false);
	~TimeProfiler();

	/*
		Start and name an interval, has to endInterval before a new startInterval can be called/created
	*/
	void startInterval(const char* name);

	/*
		Used to end a startedInterval
	*/
	void endInterval();

	/*
		Has to be called every frame to indicate when your done profiling, to update and draw gui!
	*/
	void endFrame();

	const float * getTimes();

private:
	std::string title;

	Timer timer;
	Timer dtTimer;

	bool inInterval;
	unsigned currentInterval;
	unsigned intervalCount;

	float * statistics;
	void resetIntervalCount();

	std::vector<std::pair<float, std::string>> times;
};

