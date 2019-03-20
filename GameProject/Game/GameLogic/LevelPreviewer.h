#pragma once

#include <Engine/Collision/CollisionHandler.h>
#include <Engine/Components/Camera.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/Rendering/Lighting/LightManager.h>
#include <Game/Components/PathTreader.h>
#include <Game/GameLogic/ReplaySystem.h>
#include <Game/GameLogic/TargetManager.h>
#include <Game/Level/LevelParser.h>
#include <Game/Level/LevelStructure.h>
#include <Game/Level/Level.h>
#include <string>

class LevelPreviewer
{
public:
    LevelPreviewer(EntityManager* entityManager);
    ~LevelPreviewer();

    void setLevel(const std::string& levelName);

    void render();
    void update(float dt);

    Level& getLevel();

    void stopReplaying();

private:
    LevelParser levelParser;


    Level level;

    // Level systems
    EntityManager* entityManager;
    ReplaySystem replaySystem;
    LevelStructure levelStructure;
    TargetManager* targetManager;
    CollisionHandler collisionHandler;
    LightManager lightManager;

    Entity* cameraEntity;
    Camera* camera;

    // Replay
    float elapsedTime, replayLength;
    bool replayExists;

    Entity* replayArrow;
    PathTreader* pathTreader;
};
