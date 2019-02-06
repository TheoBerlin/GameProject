#include <Catch/catch.hpp>

#include <Engine/Entity/Entity.h>
#include <Engine/Entity/EntityMatrix.h>
#include <Game/Components/ArrowGuider.h>
#include <Engine/Components/Camera.h>
#include <Engine/Events/Events.h>
#include <Engine/Events/EventBus.h>

TEST_CASE("Arrow guider") {
    // Create an entity and an ArrowGuider component
    glm::vec3 startingPosition(0.0f, 0.0f, 0.0f);
    glm::vec3 startingDirection(0.0f, 0.0f, 1.0f);
    float startingSpeed = 0.1f;

    Entity* arrowEntity = new Entity();

    Camera* arrowCam = new Camera(arrowEntity);
    ArrowGuider* arrowGuider = new ArrowGuider(arrowEntity, startingPosition, startingDirection, startingSpeed);

    SECTION("Travels in a linear path with linear speed") {
        float dt = 10.0f;
        arrowGuider->startGuiding();
        arrowGuider->update(dt);

        glm::vec3 newPos = arrowEntity->getMatrix()->getPosition();
        glm::vec3 expectedPos = startingPosition + startingDirection * startingSpeed * dt;

        REQUIRE(Approx(newPos.x - expectedPos.x) == 0.0f);
        REQUIRE(Approx(newPos.y - expectedPos.y) == 0.0f);
        REQUIRE(Approx(newPos.z - expectedPos.z) == 0.0f);

        SECTION("Stores its travel path") {
            std::vector<glm::vec3> expectedPositions;
            expectedPositions.push_back(startingPosition);
            expectedPositions.push_back(expectedPos);

            // Choose a dt which makes every update(dt) trigger the position to be stored
            float posStoreFrequency = arrowGuider->getPosStoreFrequency();
            dt = 1.001f / posStoreFrequency;

            // Trigger a number of positions to be stored
            unsigned int updateCount = 10;

            for (unsigned int i = 0; i < updateCount; i += 1) {
                arrowGuider->update(dt);

                expectedPos = newPos + startingDirection * startingSpeed * dt;
                expectedPositions.push_back(expectedPos);
            }

            // The position storing frequency should not change when the arrow is not turning
            REQUIRE(Approx(posStoreFrequency - arrowGuider->getPosStoreFrequency()) == 0.0f);

            // Check that the positions are stored
            std::vector<glm::vec3> storedPositions = arrowGuider->getStoredPositions();

            REQUIRE(storedPositions.size() == expectedPositions.size());

            // Check that the positions are correct
            // (with some error margin because predicting the exact points is difficult)
            for (unsigned int i = 0; i < updateCount; i += 1) {
                REQUIRE(Approx(storedPositions.at(i).x - expectedPositions.at(i).x) <= 0.2f);
                REQUIRE(Approx(storedPositions.at(i).y - expectedPositions.at(i).y) <= 0.2f);
                REQUIRE(Approx(storedPositions.at(i).z - expectedPositions.at(i).z) <= 0.2f);
            }

            arrowGuider->stopGuiding();
        }
    }

    SECTION("Can be aimed using mouse input") {
        MouseMoveEvent mouseMoveEvent(100.0, 0.0);
        arrowGuider->startGuiding();

        EventBus::get().publish(&mouseMoveEvent);

        arrowGuider->update(0.01f);

        glm::vec3 newDir = arrowGuider->getDirection();

        // Horizontal mouse movement should redirect the arrow horizontally
        REQUIRE(Approx(newDir.x - startingDirection.x) != 0.0f);
        // Horizontal mouse movement should not cause the arrow to pitch
        REQUIRE(Approx(newDir.y - startingDirection.y) == 0.0f);

        SECTION("Changes camera settings when turning") {
            // Make sure that the arrow isn't already turning
            arrowGuider->update(100.0f);

            float firstFOV = arrowCam->getFOV();
            glm::vec3 firstOffset = arrowCam->getOffset();

            // Turn the arrow
            MouseMoveEvent mouseMoveEvent(1000.0, 0.0);
            EventBus::get().publish(&mouseMoveEvent);

            arrowGuider->update(0.1f);

            // Check that the camera settings have changed after a small amount of time
            float secondFOV = arrowCam->getFOV();
            glm::vec3 secondOffset = arrowCam->getOffset();

            REQUIRE(secondFOV > firstFOV);
            REQUIRE(glm::length(secondOffset) > glm::length(firstOffset));

            arrowGuider->stopGuiding();
        }

        SECTION("Turns over time") {
            arrowGuider->update(10.0f);

            // See if the direction has changed from the previous test
            glm::vec3 oldDir = newDir;
            newDir = arrowGuider->getDirection();

            REQUIRE(Approx(newDir.x - oldDir.x) != 0.0f);

            // Horizontal mouse movement should not cause the arrow to pitch
            REQUIRE(Approx(newDir.y - oldDir.y) == 0.0f);

            SECTION("Slows down turning over time") {
                // Start turning the arrow
                MouseMoveEvent mouseMoveEvent(700.0, 0.0);
                EventBus::get().publish(&mouseMoveEvent);

                float oldTurningSpeed = arrowGuider->getTurningSpeed();

                for (unsigned int i = 0; i < 3; i += 1) {
                    // Check that the turning speed is decreased over time
                    arrowGuider->update(0.1f);
                    float newTurningSpeed = arrowGuider->getTurningSpeed();

                    REQUIRE(oldTurningSpeed > newTurningSpeed);

                    oldTurningSpeed = newTurningSpeed;
                }

                arrowGuider->stopGuiding();
            }

            SECTION("Increases its storing frequency when turning faster") {
                MouseMoveEvent mouseMoveEvent(700.0, 0.0);

                EventBus::get().publish(&mouseMoveEvent);

                float firstPosStoreFrequency = arrowGuider->getPosStoreFrequency();

                // Turning the arrow and updating to refresh storing frequency
                arrowGuider->update(0.01f);

                float secondPosStoreFrequency = arrowGuider->getPosStoreFrequency();

                REQUIRE(secondPosStoreFrequency > firstPosStoreFrequency);

                // Letting the arrow slow down its turning and decrease storing frequency
                arrowGuider->update(100.0f);

                REQUIRE(arrowGuider->getPosStoreFrequency() < secondPosStoreFrequency);
            }
        }
    }
}
