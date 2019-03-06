#pragma once

#include <vector>

#include "../Rendering/GLAbstraction/RenderingResources.h"

class Mesh;
class Entity;

class Model
{
    public:
        Model();
        ~Model();

        void addMesh(Mesh* mesh);
		void addMaterial(Material material);
        Material& getMaterial(unsigned short index);

        size_t meshCount();
        size_t materialCount();

		/*
			Add entity to models rendering group, return index in vector
		*/
		unsigned addEntity(Entity * entity);
		/*
			Remove entity to models rendering group using index in vector
		*/
		void removeEntity(unsigned index);
		/*
			Get rendering group
		*/
		const std::vector<Entity*>& getRenderingGroup() const;

		/*
			inits vertex buffer for meshes rendering groups transforms
		*/
		void initInstancing();

		/*
			GENERAL USAGE: Can be used to initilize a VBO on a specfic mesh
		*/
		void initInstancing(unsigned meshIndex, const void * data, size_t dataSize, const AttributeLayout & attributeLayout);

		/*
			Updates all meshes second vbo with entities transform data
		*/
		void updateInstancingSpecificData(unsigned renderingGroupIndex);

		/*
			GENERAL USAGE: This function can be used to specify the data, mesh and vbo which you want to update.
		*/
		void updateInstancingSpecificData(const void * data, size_t dataSize, unsigned offset, unsigned meshIndex, unsigned vboIndex);

		/*
			Updates vertex buffer for models meshes with rendering groups transforms
		*/
		void updateInstancingData();

        // Returns pointer to indexed mesh
        Mesh* getMesh(size_t index);

    private:
        std::vector<Mesh*> meshes;
        std::vector<Material> materials;
		/*
			Contains entities using this model
		*/
		std::vector<Entity*> renderingGroup;
};
