#pragma once

#include <Engine/Components/FreeMove.h>
#include <Engine/GUI/Button.h>
#include <Engine/GUI/Panel.h>
#include <Game/GameLogic/Phases/Phase.h>
#include <Game/Components/PathTreader.h>
#include <Game/Components/PathVisualizer.h>

class GuidingPhase;
class AimPhase;

class ReplayPhase : public Phase
{
public:
    ReplayPhase(GuidingPhase* guidingPhase);
	~ReplayPhase();

    void update(const float& dt);

    Entity* getFreeCam() const;

    Entity* getReplayArrow() const;
    PathVisualizer* getPathVisualizer() const;

private:
    void handleKeyInput(KeyEvent* event);

    void beginAimTransition();
    void finishAimTransition(CameraTransitionEvent* event);

    void setupGUI();

    void handleTimeBarClick();

    Entity* freeCam;
    FreeMove* freeMove;

    Entity* replayArrow;
    PathTreader* pathTreader;
    PathVisualizer* pathVisualizer;

    // This should be read from the level struct in the future
    const glm::vec3 arrowPos = glm::vec3(1.0f, 1.0f, 16.0f);

    // Total flight time retrieved from guiding phase
    float flightTime, replayTime;

    // GUI data
    unsigned int screenWidth, screenHeight;
    // Background time bar
    Button* timeBarBack;
    // Foreground time bar, displays played time
    Panel* timeBarFront;

    // Time bar position and size factors, relative to screen size
    const float timeBarSidePadding = 1.0f/20.0f;
    const float timeBarBottomPadding = 1.0f/20.0f;
    const float timeBarHeightFactor = 1.0f/40.0f;

    // Time bar visuals
    const glm::vec4 timeBarBackColor = {0.9686f, 0.7725f, 0.2039f, 1.0f};
    const glm::vec4 timeBarFrontColor = {0.6588f, 0.4784f, 0.0f, 1.0f};

    // Time bar slider (purely cosmetic)
    Panel* timeBarSlider;

    // Size relative to screen height
    const glm::vec2 sliderSizeFactors = {timeBarHeightFactor, timeBarHeightFactor};
    glm::uvec2 sliderSize;
};