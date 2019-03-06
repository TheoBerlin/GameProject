#pragma once

#include <Engine/AssetManagement/ModelLoader.h>
#include <Engine/Components/Component.h>
#include <Engine/Entity/Entity.h>
#include <Engine/Entity/EntityManager.h>
#include <Game/Components/ComponentResources.h>
#include <vector>

class PathVisualizer : public Component
{
public:
    PathVisualizer(Entity* host, EntityManager* em);
    ~PathVisualizer();

    void update(const float& dt);

    // Sets a singular path to be visualized
    void addPath(const std::vector<KeyPoint>& path);

    // Removes all visualizer entities
    void removeVisualizers();

private:
    // Used to add entities to render path key points
    EntityManager* em;

    unsigned int pathCount;
    // Amount of points in each path
    std::vector<unsigned int> pointCount;
};
