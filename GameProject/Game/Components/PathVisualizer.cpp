#include "PathVisualizer.h"

#include <Utils/Logger.h>

PathVisualizer::PathVisualizer(Entity* host, EntityManager* em)
    :Component(host, "PathVisualizer")
{
    this->em = em;

    pathCount = 0;
}

PathVisualizer::~PathVisualizer()
{
}

void PathVisualizer::update(const float& dt)
{
}

void PathVisualizer::addPath(const std::vector<KeyPoint>& path)
{
    pathCount += 1;

    pointCount.push_back(path.size());

    // Add an entity for each key point in the path
    for (unsigned int i = 0; i < path.size(); i += 1) {
        std::string entityName = "Path" + std::to_string(pathCount - 1) + "Point" + std::to_string(i);

        Entity* visualizer = this->em->addTracedEntity(entityName);

        // Set visualizer transform and model
        visualizer->getTransform()->setPosition(path.at(i).Position);
	    visualizer->getTransform()->setScale(glm::vec3(0.1f, 0.1f, 0.1f));

	    visualizer->setModel(ModelLoader::loadModel("./Game/assets/Cube.fbx"));
    }
}

void PathVisualizer::removeVisualizers()
{
    for (unsigned int path = 0; path < pathCount; path += 1) {
        for (unsigned int point = 0; point < pointCount.at(path); point += 1) {
            std::string entityName = "Path" + std::to_string(path) + "Point" + std::to_string(point);

            if (!this->em->removeTracedEntity(entityName)) {
                LOG_WARNING("Failed to remove path visualizer: [%s]", entityName.c_str());
            }
        }
    }
}
