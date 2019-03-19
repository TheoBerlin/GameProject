#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

struct CollisionReplay;
struct KeyPoint;
struct Level;
class PathTreader;

class ReplayParser
{
public:
    static void writeReplay(const std::string& levelName, const std::vector<CollisionReplay>& collisions, const std::vector<KeyPoint>& path);
    // Returns false if no replay exists
    static bool readReplay(const Level& level, const std::string& levelName, std::vector<CollisionReplay>& collisions, std::vector<KeyPoint>& path);

private:
    static void writeCollisions(nlohmann::json& replayFile, const std::vector<CollisionReplay>& collisions);
    static void writePath(nlohmann::json& replayFile, const std::vector<KeyPoint>& path);

    static bool readCollisions(const Level& level, const nlohmann::json& replayFile, std::vector<CollisionReplay>& collisions);
    static void readPath(const nlohmann::json& replayFile, std::vector<KeyPoint>& path);

    static void readVec3(const nlohmann::json& file, glm::vec3& vec);
};
