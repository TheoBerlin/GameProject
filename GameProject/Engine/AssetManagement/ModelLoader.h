#pragma once

#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "TextureManager.h"
#include <map>
#include "../../Utils/Logger.h"

class ModelLoader
{
public:
	~ModelLoader();

    static Model* loadModel(std::string fileName);

	static std::vector<Model*> getModels();
    static void unloadAllModels();
    static size_t modelCount();

private:
    static std::map<std::string, Model*> loadedModels;

    // Functions for processing assimp's loaded data to our own resource formats

    // Loads all textures in a material given a type (eg. diffuse)
    static void processMaterial(aiMaterial* material, Model* model, aiTextureType type, const std::string& directory);
    static void processNode(const aiScene* scene, aiNode* node, Model* model);
    static void processMesh(aiMesh* assimpMesh, Model* model);
    static TextureType convertTextureType(const aiTextureType& assimpType);
};
