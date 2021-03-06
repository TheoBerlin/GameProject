#pragma once

#include "glm/glm.hpp"
#include "nlohmann/json.hpp"

#include <Game/Components/PathTreader.h>
#include <Game/Level/Level.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/AssetManagement/ModelLoader.h>

#include <fstream>
#include <iostream>
#include <iomanip>

namespace json = nlohmann;

class LevelParser {
private:
	json::json jsonFile;

	void readEntityTargets(Level& level);
	void readEntityProps(Level& level);
	void readEntityWalls(Level& level);
	void readPlayer(Level& level);
	void readLights(Level& level);

	void writeEntityProps(Level& level);
	void writeEntityTargets(Level& level);
	void writePlayer(Level& level);
	void writeWalls(Level& level);
	void writeLight(Level& level);

	void readVec4(json::json& file, glm::vec4& vec);
	void readVec3(json::json& file, glm::vec3& vec);
	void readVec2(json::json& file, glm::vec2& vec);

	void readPath(json::json& file, Entity* entity, std::vector<KeyPoint>& path);
	void readCameraSetting(json::json& file, CameraSetting& camera);

	json::json::value_type readValueGeneric(json::json& file, std::string value);
	template <class T>
	T readValue(json::json& file, std::string value);

	void createCollisionBodies(Level& level);

public:
	void readLevel(std::string file, Level& level);
	void readLevelInfo(std::string file, std::vector<std::string>& info);
	void writeLevel(std::string file, Level& level);
	void writeScore(Level& level);

	void readMetadata(Level& level);
};

template<class T>
inline T LevelParser::readValue(json::json & file, std::string value)
{
	return (T)readValueGeneric(file, value);
}
