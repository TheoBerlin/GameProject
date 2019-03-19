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
	// Set heighest score for level
	void setHighscore(const unsigned highscore);
	// Get highest score for level
	int getHighscore() const;
	//Get the optimal time for the level
	float getOptimalTime() const;
	// Get total time
	float getTotalTime() const;
	// Get targets hit
	unsigned getTargetsHit() const;
	// Reset score
	void resetScore();

	// Show score GUI
	void showResults(Level& level, const std::function<void()>& retry);
	// Returns true if results are being shown
	bool resultsVisible() const;

	void removeResultsGUI(Level& level);

	// Callback function for results gui
	void toggleGuiMinimize();

private:
	bool counting;
	bool newHighscore;
	unsigned totalScore;
	unsigned highscore;
	unsigned targetsHit;
	float totalTime;
	float optimalTime;

	Timer* timer;

	// Results panel varibles
	bool showGui;
	bool minimized;
	Panel* smallPanel;
	Panel* bigPanel;

};
