#include "TimeProfiler.h"

#include <string>
#include "../Engine/Imgui/imgui.h"
#include "../Engine/Config.h"
#include "GL/glew.h"

#define BUFFER_OFFSET(i)    ((void*)NULL + (i))

TimeProfiler::TimeProfiler(const std::string & title, bool displayGUI)
{
	this->inInterval = false;
	this->title = title;
	this->currentInterval = 0;
	this->intervalCount = 0;
	this->statistics = nullptr;
	dtTimer.start();
	glGenQueries(1, &this->queryID);
}


TimeProfiler::~TimeProfiler()
{
	if(this->statistics != nullptr)
		delete[] this->statistics;
	glDeleteQueries(1, &this->queryID);
}

void TimeProfiler::startInterval(const char* name)
{

	if (this->currentInterval == this->times.size()) {
		this->times.push_back(std::make_pair(0.0f, name));
		this->intervalCount++;
		glBeginQuery(GL_TIME_ELAPSED, this->queryID);
	}

	if (!this->inInterval) {
		this->timer.restart();
		this->inInterval = true;
	}
}

void TimeProfiler::endInterval()
{
	if (this->currentInterval < this->intervalCount && this->inInterval) {
		this->timer.stop();
		this->times[this->currentInterval].first += (float)this->timer.restart() * 1000.0f;
		this->times[this->currentInterval].first /= 2.0;
		this->inInterval = false;
		this->currentInterval++;
		glEndQuery(GL_TIME_ELAPSED);
	}
}

void TimeProfiler::endFrame()
{
	/*
		Register interval calls has changed and reset interval
	*/
	this->resetIntervalCount();


	#ifdef IMGUI
	ImGui::Begin(("Time Profiler: " + this->title).c_str());

	const float * times = this->getTimes();
	ImGui::Text("Time Intervals");
	ImGui::PlotHistogram("", times, this->intervalCount, 0, NULL, 0.0f, 3.0f, ImVec2(400, 100));

	float totalTime = 0.0f;

	for (unsigned i = 0; i < this->times.size(); i++)
	{
		ImGui::Text((std::to_string(i) + ": " + this->times[i].second).c_str());
		ImGui::SameLine(300.0f);
		ImGui::Text((std::to_string(times[i]) + " ms").c_str());
		totalTime += times[i];
	}
	

	ImGui::Text("Total Time: ");
	ImGui::SameLine(300.0f);
	ImGui::Text((std::to_string(totalTime) + " ms").c_str());

	ImGui::Text("GPU time: ");
	ImGui::SameLine(300.0f);
	GLint value;
	glGetQueryObjectiv(this->queryID, GL_QUERY_RESULT, &value);
	ImGui::Text((std::to_string(value / 1000000) + " ms").c_str());

	ImGui::End();

	#endif // IMGUI

}

const float * TimeProfiler::getTimes()
{
	return this->statistics;
}

void TimeProfiler::resetIntervalCount()
{
	static double refreshTime = 1.0;
	static double passedTime = 0.0;
	

	if (this->statistics == nullptr) {
		//Delete old statics if they exist
		if (this->statistics != nullptr) {
			delete[] this->statistics;
			this->statistics = nullptr;
		}

		// Update statitics with new data
		this->intervalCount = this->currentInterval;
		this->statistics = new float[this->intervalCount];

		for (unsigned i = 0; i < this->intervalCount; i++)
			this->statistics[i] = this->times[i].first;
	}

	dtTimer.stop();
	passedTime += dtTimer.restart();
	
	// Update statistics every second
	if (passedTime >= refreshTime) {
		for (unsigned i = 0; i < this->intervalCount; i++)
			this->statistics[i] = this->times[i].first;

		passedTime = 0.0;
	}

	/*
		Reset to interval 0 for next frame
	*/
	this->currentInterval = 0;
}
