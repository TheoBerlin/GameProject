#include <Catch/catch.hpp>

#include <Engine/Entity/Entity.h>
#include <Engine/Entity/EntityMatrix.h>
#include <Game/Components/ArrowGuider.h>
#include <Engine/Events/Events.h>
#include <Engine/Events/EventBus.h>

TEST_CASE("Arrow guider") {
    // Create an entity and an ArrowGuider component
    glm::vec3 startingPosition(0.0f, 0.0f, 0.0f);
    glm::vec3 startingDirection(0.0f, 0.0f, 1.0f);
    float startingSpeed = 0.1f;

    EntityMatrix startingTransform;
    startingTransform.setPosition(startingPosition);

    Entity* arrowEntity = new Entity();

    ArrowGuider* arrowGuider = new ArrowGuider(arrowEntity, startingTransform, startingDirection, startingSpeed);

    SECTION("Travels in a linear path with linear speed") {
        float dt = 10.0f;
        arrowGuider->startGuiding();
        arrowGuider->update(dt);

        glm::vec3 newPos = arrowEntity->getMatrix()->getPosition();
        glm::vec3 expectedPos = startingPosition + startingDirection * startingSpeed * dt;

        REQUIRE(Approx(newPos.x - expectedPos.x) == 0.0f);
        REQUIRE(Approx(newPos.y - expectedPos.y) == 0.0f);
        REQUIRE(Approx(newPos.z - expectedPos.z) == 0.0f);

        arrowGuider->stopGuiding();
    }

    SECTION("Can be aimed using mouse input") {
        MouseMoveEvent mouseMoveEvent(100.0, 0.0);
        arrowGuider->startGuiding();

        EventBus::get().publish(&mouseMoveEvent);

        arrowGuider->update(0.1f);

        glm::vec3 newDir = arrowGuider->getDirection();

        CAPTURE(newDir.x, newDir.y, newDir.z);

        // Horizontal mouse movement should redirect the arrow horizontally
        REQUIRE(Approx(newDir.x - startingDirection.x) != 0.0f);
        // Horizontal mouse movement should not cause the arrow to pitch
        REQUIRE(Approx(newDir.y - startingDirection.y) == 0.0f);

        SECTION("Turns over time") {
            arrowGuider->update(10.0f);

            // See if the direction has changed from the previous test
            glm::vec3 oldDir = newDir;
            newDir = arrowGuider->getDirection();

            REQUIRE(Approx(newDir.x - oldDir.x) != 0.0f);
            // Horizontal mouse movement should not cause the arrow to pitch
            REQUIRE(Approx(newDir.y - oldDir.y) == 0.0f);
        }
    }
}
