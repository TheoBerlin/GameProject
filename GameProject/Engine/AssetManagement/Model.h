#pragma once

#include "GL/glew.h"
#include <vector>
#include "Mesh.h"

class Model
{
    public:
        Model();
        ~Model();

        void addMesh(Mesh* mesh);
		void addMaterial(Material material);
        Material getMaterial(unsigned short index);

        size_t meshCount();
        size_t materialCount();

        // Returns pointer to indexed mesh
        Mesh* getMesh(size_t index);

    private:
        unsigned int VAO;
        std::vector<Mesh*> meshes;
        std::vector<Material> materials;
};
