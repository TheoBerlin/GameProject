#include "Model.h"

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

void Model::initInstancing(unsigned meshIndex, const void * data, size_t dataSize, const AttributeLayout & attributeLayout)
{
	if(meshIndex >= 0 && meshIndex < this->meshes.size())
		this->meshes[meshIndex]->initInstancing(data, dataSize, attributeLayout);
}

void Model::updateInstancingSpecificData(unsigned renderingGroupIndex)
{
	size_t amount = this->renderingGroup.size();
	glm::mat4* matrix = new glm::mat4;

	// If entity is separated, use the saved transformation, else use the current transform from the entity.
	if (this->renderingGroup[renderingGroupIndex]->isTransformSeparated())
		this->renderingGroup[renderingGroupIndex]->getPausedTransform()->getMatrix(matrix);
	else
		this->renderingGroup[renderingGroupIndex]->getTransform()->getMatrix(matrix);

	for (size_t i = 0; i < this->meshCount(); i++)
	{
		Mesh* mesh = this->getMesh(i);

		mesh->updateInstancingData(&matrix[0], sizeof(glm::mat4), sizeof(glm::mat4) * renderingGroupIndex);
	}

	delete matrix;
}

void Model::updateInstancingSpecificData(const void * data, size_t dataSize, unsigned offset, unsigned meshIndex, unsigned vboIndex)
{
	Mesh* mesh = this->getMesh(meshIndex);

	mesh->updateInstancingData(data, dataSize, offset, vboIndex);
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

void Model::setName(std::string name)
{
	this->name = name;
}

std::string Model::getName() const
{
	return name;
}
