#include "Model.h"

#include "Utils/Logger.h"
#include "Mesh.h"
#include "Engine/Entity/Entity.h"

Model::Model()
{
}

Model::~Model()
{
    // Delete all meshes
    for (unsigned short i = 0; i < meshes.size(); i += 1) {
		delete meshes.at(i);
    }
}

void Model::addMesh(Mesh* mesh)
{
    meshes.push_back(mesh);
	/*
		Init instancing verter buffer
	*/
	mesh->initInstancing(NULL, 0);
}

void Model::addMaterial(Material material)
{
    materials.push_back(material);
}

Material& Model::getMaterial(unsigned short index)
{
    return materials.at(index);
}

size_t Model::meshCount()
{
    return meshes.size();
}

size_t Model::materialCount()
{
    return materials.size();
}

unsigned Model::addEntity(Entity * entity)
{
	unsigned index = this->renderingGroup.size();
	this->renderingGroup.push_back(entity);

	return index;
}

void Model::removeEntity(unsigned index)
{
	if (index < this->renderingGroup.size()) {
		auto it = this->renderingGroup.erase(this->renderingGroup.begin() + index);

		while (it != this->renderingGroup.end()) {
			(*it)->setRenderingGroupIndex((*it)->getRenderingGroupIndex() - 1);
			it++;
		}
	}
}

const std::vector<Entity*>& Model::getRenderingGroup() const
{
	return this->renderingGroup;
}

void Model::initInstancing()
{
	size_t amount = this->renderingGroup.size();
	glm::mat4* matrices = new glm::mat4[amount];

	for (size_t i = 0; i < amount; i++) {
		this->renderingGroup[i]->getTransform()->getMatrix(&matrices[i]);
	}

	for (size_t i = 0; i < this->meshCount(); i++)
	{
		Mesh* mesh = this->getMesh(i);

		mesh->initInstancing(&matrices[0], amount * sizeof(glm::mat4));
	}

	delete[] matrices;
}

void Model::updateInstancingSpecificData(unsigned renderingGroupIndex)
{
	size_t amount = this->renderingGroup.size();
	glm::mat4* matrix = new glm::mat4;

	this->renderingGroup[renderingGroupIndex]->getTransform()->getMatrix(matrix);

	for (size_t i = 0; i < this->meshCount(); i++)
	{
		Mesh* mesh = this->getMesh(i);

		mesh->updateInstancingData(&matrix[0], sizeof(glm::mat4), sizeof(glm::mat4) * renderingGroupIndex);
	}

	delete matrix;
}

void Model::updateInstancingData()
{
	size_t amount = this->renderingGroup.size();
	glm::mat4* matrices = new glm::mat4[amount];

	for (size_t i = 0; i < amount; i++) {
		this->renderingGroup[i]->getTransform()->getMatrix(&matrices[i]);
	}

	for (size_t i = 0; i < this->meshCount(); i++)
	{
		Mesh* mesh = this->getMesh(i);

		mesh->updateInstancingData(&matrices[0], amount * sizeof(glm::mat4));
	}

	delete[] matrices;
}

Mesh* Model::getMesh(size_t index)
{
    return meshes.at(index);
}
