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

	/*void writePosition(int index, glm::vec3 position);
	void writeName(int index, std::string name);*/

	void readEntityTargets(Level& level);
	void readEntityBoxes(Level& level);
	void readEntityWalls(Level& level);
	void readEntityFloor(Level& level);
	void readPlayer(Level& level);
	void readMetadata(Level& level);
	void readLights(Level& level);

	void readVec4(json::json& file, glm::vec4& vec);
	void readVec3(json::json& file, glm::vec3& vec);
	void readVec2(json::json& file, glm::vec2& vec);

	void readPath(json::json& file, Entity* entity, std::vector<KeyPoint>& path);
	void readCameraSetting(json::json& file, CameraSetting& camera);

	template <class T>
	T readValue(json::json& file, std::string value);

	void createCollisionBodies(Level& level);

public:
	//void writeToFile(std::string file, EntityManager *entityManager);
	void readLevel(std::string file, Level& level);

};

template<class T>
inline T LevelParser::readValue(json::json & file, std::string value)
{
	if (!file[value].empty()) {
		return file[value];
	}
	else {
		printf("Value of %s not found in level!", value.c_str());
		return T();
	}
}
