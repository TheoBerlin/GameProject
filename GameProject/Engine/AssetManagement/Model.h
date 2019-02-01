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

        // Returns the amount of meshes comprising the model
        size_t meshCount();

        // Returns pointer to indexed mesh
        Mesh* getMesh(size_t index);

    private:
        unsigned int VAO;
        std::vector<Mesh*> meshes;
        std::vector<Material> materials;
};
