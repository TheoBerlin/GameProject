#pragma once

#include "GL/glew.h"
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
			Updates specific entities transform in vertexbuffer on model
		*/
		void updateInstancingSpecificData(unsigned renderingGroupIndex);

		/*
			Updates vertex buffer for models meshes rendering groups transforms
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
