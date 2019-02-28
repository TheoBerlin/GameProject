#pragma once

class Timer;

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

private:
	bool counting;
	unsigned totalScore;
	unsigned targetsHit;
	float totalTime;
	float optimalTime;

	Timer* timer;
};

