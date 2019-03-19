#include "ReplayParser.h"

#include <Engine/Entity/Entity.h>
#include <Game/Components/ComponentResources.h>
#include <Game/GameLogic/ReplaySystem.h>
#include <Game/Level/Level.h>
#include <Utils/Logger.h>
#include <fstream>

void ReplayParser::writeReplay(const std::string& levelName, const std::vector<CollisionReplay>& collisions, const std::vector<KeyPoint>& path)
{
    nlohmann::json replayFile;

    // Write json string
    writeCollisions(replayFile, collisions);
    writePath(replayFile, path);

    // Create file name, remove .json from level name
    size_t extensionIndex = levelName.find_last_of('.');
    std::string replayName = levelName.substr(0, extensionIndex) + "_Replay.json";

    LOG_INFO("Writing replay file: [%s]", replayName.c_str());

    // Write json string to file
    std::ofstream oFile;
	oFile.open(replayName);
	oFile << std::setw(4) << replayFile << std::endl;
}

bool ReplayParser::readReplay(const Level& level, const std::string& levelName, std::vector<CollisionReplay>& collisions, std::vector<KeyPoint>& path)
{
    // Get replay's file name using level name, remove .json from level name
    size_t extensionIndex = levelName.find_last_of('.');
    std::string replayName = levelName.substr(0, extensionIndex) + "_Replay.json";

    std::ifstream iFile;
	iFile.open(replayName);

	if (!iFile.is_open()) {
        LOG_INFO("No replay exists for level: [%s]", levelName.c_str());
        return false;
    }

    nlohmann::json replayFile;

    try {
        iFile >> replayFile;
    } catch (const std::exception e) {
        LOG_WARNING("Failed to load level replay file with error: [%s]", e.what());
        return false;
    }

    collisions.clear();
    path.clear();

    // The file has been loaded into a json object
    if (!readCollisions(level, replayFile, collisions)) {
        return false;
    }

    readPath(replayFile, path);

    return true;
}

void ReplayParser::writeCollisions(nlohmann::json& replayFile, const std::vector<CollisionReplay>& collisions)
{
    for (unsigned int collisionIndex = 0; collisionIndex < collisions.size(); collisionIndex += 1) {
        // Extract name of hit target
        std::string targetName = collisions[collisionIndex].event.entity2->getName();

        replayFile["Collisions"][collisionIndex]["Target"] = targetName;
        replayFile["Collisions"][collisionIndex]["TargetShape"] = collisions[collisionIndex].event.shape2->getCollisionCategoryBits();
        replayFile["Collisions"][collisionIndex]["Time"] = collisions[collisionIndex].time;
    }
}

void ReplayParser::writePath(nlohmann::json& replayFile, const std::vector<KeyPoint>& path)
{
    for (unsigned int pathIndex = 0; pathIndex < path.size(); pathIndex += 1) {
        glm::vec3 position = path[pathIndex].Position;
        replayFile["Path"][pathIndex]["Position"] = {position.x, position.y, position.z};

        glm::vec3 up = path[pathIndex].UpVector;
        replayFile["Path"][pathIndex]["Up"] = {up.x, up.y, up.z};

        replayFile["Path"][pathIndex]["T"] = path[pathIndex].t;
    }
}

bool ReplayParser::readCollisions(const Level& level, const nlohmann::json& replayFile, std::vector<CollisionReplay>& collisions)
{
    unsigned int collisionCount = replayFile["Collisions"].size();

    collisions.resize(collisionCount);

    // Used to translate target name to an entity
    std::vector<Entity*> targetEntities = level.targetManager->getTargetEntities();

    for (unsigned int collisionIndex = 0; collisionIndex < collisionCount; collisionIndex += 1) {
        std::string targetName = replayFile["Collisions"][collisionIndex]["Target"];

        // Translate target name to entity
        Entity* targetEntity;

        unsigned int targetIndex = 0;

        while (targetIndex < targetEntities.size() && targetEntities[targetIndex]->getName() != targetName) {
            targetIndex += 1;
        }

        // The hit target was not found
        if (targetIndex == targetEntities.size()) {
            LOG_ERROR("Target [%s] was not found in the replay file of [%s]", level.levelName.c_str());
            return false;
        }

        targetEntity = targetEntities[targetIndex];

        // Time of collision
        float time = replayFile["Collisions"][collisionIndex]["Time"];

        collisions[collisionIndex].event.entity2 = targetEntity;

        // Identify collision shape 2 (which part of the target that was hit)
        unsigned int shapeCategory = replayFile["Collisions"][collisionIndex]["TargetShape"];

        reactphysics3d::ProxyShape* targetShape = targetEntity->getCollisionBody()->getProxyShapesList();

        while (targetShape != nullptr && targetShape->getCollisionCategoryBits() != shapeCategory) {
            targetShape = targetShape->getNext();
        }

        collisions[collisionIndex].event.shape2 = targetShape;
        collisions[collisionIndex].time = time;
    }

    return true;
}

void ReplayParser::readPath(const nlohmann::json& replayFile, std::vector<KeyPoint>& path)
{
    unsigned int pathSize = replayFile["Path"].size();

    path.resize(pathSize);

    for (unsigned int pathIndex = 0; pathIndex < pathSize; pathIndex += 1) {
        glm::vec3 position;
        readVec3(replayFile["Path"][pathIndex]["Position"], position);

        glm::vec3 up;
        readVec3(replayFile["Path"][pathIndex]["Up"], up);

        float time = replayFile["Path"][pathIndex]["T"];

        path[pathIndex] = KeyPoint(position, up, time);
    }
}

void ReplayParser::readVec3(const nlohmann::json& file, glm::vec3& vec)
{
    for (unsigned int i = 0; i < 3; i += 1) {
        vec[i] = file[i];
    }
}
