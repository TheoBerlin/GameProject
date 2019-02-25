#pragma once

#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "TextureManager.h"
#include <unordered_map>

class CollisionHandler;
class ModelLoader
{
public:
	~ModelLoader();

    static Model* loadModel(std::string fileName);
	static Model* loadModel(std::string fileName, CollisionHandler* ch);

	static std::vector<Model*> getModels();
	static Model* getModel(const std::string & name);
    static void unloadAllModels();
    static size_t modelCount();

private:
    static std::unordered_map<std::string, Model*> loadedModels;

	struct MeshData
	{
		std::vector<Vertex>* vertices;
		std::vector<unsigned int>* indices;
		unsigned int materialIndex;
	};

    // Functions for processing assimp's loaded data to our own resource formats

    // Loads all textures in a material given a type (eg. diffuse)
    static void processMaterial(aiMaterial* material, Model* model, aiTextureType type, const std::string& directory);
    static void processNode(const aiScene* scene, aiNode* node, Model* model);
    static void processMesh(aiMesh* assimpMesh, Model* model);
	static void processNode(const aiScene* scene, aiNode* node, Model* model, CollisionHandler* ch, const std::string& fileName, std::unordered_map<unsigned int, MeshData>* meshMapIn = nullptr);
	static void processMesh(aiMesh* assimpMesh, CollisionHandler* ch, const std::string& fileName, std::unordered_map<unsigned int, MeshData>& meshMap);
    static TextureType convertTextureType(const aiTextureType& assimpType);
};
