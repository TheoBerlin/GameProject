#pragma once

#include <functional>

class Timer;
class Panel;
struct Level;

class ScoreManager
{
public:
	ScoreManager();
	virtual ~ScoreManager();

	// Reset timer and start counting points
	void start();
	// Stop the counting of score
	void stop();
	// Get the current score
	unsigned getScore() const;
	// Score normal amount of points
	void score();
	// Score bonus points
	void scoreBonus();
	// Set optimal time for level
	void setOptimalTime(const float& time);
	// Get total time
	float getTotalTime() const;
	// Get targets hit
	unsigned getTargetsHit() const;

	// Show score GUI
	void showResults(Level& level, const std::function<void()>& retry);
	// Returns true if results are being shown
	bool resultsVisible() const;

	void removeResultsGUI(Level& level);

	// Callback function for results gui
	void toggleGuiMinimize();

private:
	bool counting;
	unsigned totalScore;
	unsigned targetsHit;
	float totalTime;
	float optimalTime;

	Timer* timer;

	// Results panel varibles
	float timeLeft;
	bool showGui;
	bool minimized;
	Panel* smallPanel;
	Panel* bigPanel;

};

