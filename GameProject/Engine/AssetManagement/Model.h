#pragma once

#include <vector>

#include "../Rendering/GLAbstraction/RenderingResources.h"

class Mesh;
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

        // Returns pointer to indexed mesh
        Mesh* getMesh(size_t index);

    private:
        std::vector<Mesh*> meshes;
        std::vector<Material> materials;
};
