#include <Catch/catch.hpp>

#include <Engine/Entity/Entity.h>
#include <Game/components/PathTreader.h>
#include <vector>

TEST_CASE("Path Treader") {
    // Create entity to tread path
    Entity* movingEntity = new Entity();
    PathTreader* pathTreader = new PathTreader(movingEntity);

    SECTION("Does not travel linearly in curves") {
        // Create circular path in the XZ plane
        std::vector<KeyPoint> path;

        float pathRadius = 100.0f;
        float storingFrequency = 0.4f;

        // Path length in seconds
        float pathLength = 300.0f;

        KeyPoint currentKeyPoint;

        for (float pathTime = 0.0f; pathTime <= pathLength; pathTime += 1.0f/storingFrequency) {
            glm::vec3 position;
            position.x = std::cos((pathLength/pathTime) * glm::two_pi<float>());
            position.y = 0.0f;
            position.z = std::sin((pathLength/pathTime) * glm::two_pi<float>());

            position *= pathRadius;

            currentKeyPoint.Position = position;
            currentKeyPoint.t = pathTime;

            path.push_back(currentKeyPoint);
        }

        // Set path and begin treading
        pathTreader->setPath(path);

        pathTreader->startTreading();

        // Check that the treader is not moving linearly
        glm::vec3 linearPosition;

        for (unsigned int pointIndex = 0; pointIndex < path.size() - 1; pointIndex += 1) {
            // Check right in between two key points
            pathTreader->update(storingFrequency/2.0f + FLT_EPSILON);

            linearPosition = path.at(pointIndex).Position + 0.5f * path.at(pointIndex).Position;

            glm::vec3 entityPosition = movingEntity->getTransform()->getPosition();

            REQUIRE(Approx(entityPosition.x - linearPosition.x) != 0.0f);
            REQUIRE(Approx(entityPosition.y - linearPosition.y) != 0.0f);
            REQUIRE(Approx(entityPosition.z - linearPosition.z) != 0.0f);
        }
    }
}
