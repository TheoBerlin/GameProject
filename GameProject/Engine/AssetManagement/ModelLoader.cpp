#include "ModelLoader.h"

#include "Mesh.h"
#include "../../Utils/Logger.h"

std::map<std::string, Model*> ModelLoader::loadedModels = std::map<std::string, Model*>();

ModelLoader::~ModelLoader()
{
	unloadAllModels();
}

Model* ModelLoader::loadModel(std::string fileName)
{
    Assimp::Importer importer;

    // See if the model has already been loaded
    Model* loadedModel = loadedModels[fileName];

    if (loadedModel) {
        LOG_INFO("Model [%s] already loaded", fileName.c_str());
        return loadedModel;
    }

    // The model needs to be loaded
    const aiScene* scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenNormals | aiProcess_FlipUVs);

    if (!scene || !scene->mRootNode) {
        LOG_WARNING("Assimp failed to load [%s]", fileName.c_str());

        return nullptr;
    }

    loadedModel = new Model();

    LOG_INFO("Found %d materials", scene->mNumMaterials);

    // Extract directory path for loading material and texture files
	size_t found = fileName.find_last_of('/');
    std::string directory = fileName.substr(0, found == std::string::npos ? 0 : found);
	if (directory != "") {
		directory += "/";
	}

    // Load selected textures and save texture structs in the model
    for (unsigned int i = 0; i < scene->mNumMaterials; i += 1) {
        // Load diffuse textures
        processMaterial(scene->mMaterials[i], loadedModel, aiTextureType_DIFFUSE, directory);
    }

    // Process all scene nodes recursively
    aiNode *rootNode = scene->mRootNode;
    processNode(scene, rootNode, loadedModel);

    // Save the model's pointer to avoid duplicate model data
    loadedModels[fileName] = loadedModel;

    return loadedModel;
}

std::vector<Model*> ModelLoader::getModels()
{
	std::vector<Model*> models;

	for (std::pair<std::string, Model*> pair : ModelLoader::loadedModels) {
		models.push_back(pair.second);
	}

	return models;
}

Model * ModelLoader::getModel(const std::string & name)
{
	Model* model = loadedModels[name];

	if (!model) {
		LOG_INFO("Model [%s] dosen't exist", name.c_str());
	}

	return model;
}

void ModelLoader::unloadAllModels()
{
    for (std::map<std::string, Model*>::iterator itr = loadedModels.begin(); itr != loadedModels.end(); itr++) {
        delete itr->second;
		itr->second = nullptr;
    }

}

size_t ModelLoader::modelCount()
{
    return loadedModels.size();
}

void ModelLoader::processMaterial(aiMaterial* material, Model* model, aiTextureType type, const std::string& directory)
{
    aiString texturePath;
    unsigned int textureCount = material->GetTextureCount(type);
	material->mProperties;

    Material newMaterial;

    for (unsigned int i = 0; i < textureCount; i += 1) {
        // Load texture
        material->GetTexture(type, i, &texturePath);

        // Convert aiString to std::string
        std::string convertedString(texturePath.C_Str());

        LOG_INFO("Found texture [%s]", convertedString.c_str());

        // Convert aiTextureType to TextureType
        TextureType txType = convertTextureType(type);

        Texture* texture = TextureManager::loadTexture(directory + convertedString, txType);

        newMaterial.textures.push_back(texture);
    }

    // Store material constants
    aiColor3D diffuse;
    aiColor3D specular;
	float shininess = 0.0;

    material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
    material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
	material->Get(AI_MATKEY_SHININESS, shininess);

    newMaterial.Kd.r = diffuse.r;
    newMaterial.Kd.g = diffuse.g;
    newMaterial.Kd.b = diffuse.b;

    newMaterial.Ks_factor.r = specular.r;
    newMaterial.Ks_factor.g = specular.g;
    newMaterial.Ks_factor.b = specular.b;
	newMaterial.Ks_factor.a = shininess;

    model->addMaterial(newMaterial);
}

void ModelLoader::processNode(const aiScene* scene, aiNode* node, Model* model)
{
    // Process node's meshes
    for (unsigned int i = 0; i < node->mNumMeshes; i += 1) {
        if (!scene->mMeshes[node->mMeshes[i]]->HasTextureCoords(0)) {
            LOG_WARNING("Ignoring mesh: Missing TX coordinates");
            continue;
        }
        processMesh(scene->mMeshes[node->mMeshes[i]], model);
    }

    // Recursively process child nodes
    for (unsigned int i = 0; i < node->mNumChildren; i += 1) {
        processNode(scene, node->mChildren[i], model);
    }
}

void ModelLoader::processMesh(aiMesh* assimpMesh, Model* model)
{
    // Data for the mesh
    std::vector<Vertex>* vertices = new std::vector<Vertex>;
    std::vector<unsigned int>* indices = new std::vector<unsigned int>;

    // Process vertices
    for (unsigned int i = 0; i < assimpMesh->mNumVertices; i += 1) {
        Vertex vertex;

        vertex.Position.x = assimpMesh->mVertices[i].x;
        vertex.Position.y = assimpMesh->mVertices[i].y;
        vertex.Position.z = assimpMesh->mVertices[i].z;

        vertex.Normal.x = assimpMesh->mNormals[i].x;
        vertex.Normal.y = assimpMesh->mNormals[i].y;
        vertex.Normal.z = assimpMesh->mNormals[i].z;

        vertex.TexCoords.x = assimpMesh->mTextureCoords[0][i].x;
        vertex.TexCoords.y = assimpMesh->mTextureCoords[0][i].y;

        vertices->push_back(vertex);
    }

    // Process indices
    for (unsigned int i = 0; i < assimpMesh->mNumFaces; i += 1) {
        aiFace face = assimpMesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j += 1) {
            indices->push_back(face.mIndices[j]);
        }
    }

    // Store material index
    // The materials in assimp are stored in the same order as in the new model,
    // so no conversion is needed
    unsigned int materialIndex = assimpMesh->mMaterialIndex;

    Mesh* newMesh = new Mesh(vertices, indices, materialIndex, model);

    model->addMesh(newMesh);
}

TextureType ModelLoader::convertTextureType(const aiTextureType& assimpType)
{
    switch (assimpType) {
        case aiTextureType_DIFFUSE:
            return TXTYPE_DIFFUSE;
        case aiTextureType_SPECULAR:
            return TXTYPE_SPECULAR;
        default:
            return TXTYPE_DIFFUSE;
    }
}
