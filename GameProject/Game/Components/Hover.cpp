#include "Hover.h"

#include <Engine/Entity/Entity.h>
#include <glm/gtc/random.hpp>
#include <Utils/Logger.h>

Hover::Hover(Entity* host)
    :Component(host, "Hover"),
    t1(0.0f),
    t2(glm::pi<float>())
{
    Transform* transform = host->getTransform();

    beginPos = transform->getPosition();
    endPos = beginPos + translationAnimation;

    // The beginning rotation is used when resetting the hover animation
    beginRotation = transform->getRotationQuat();

    negativeRotation = -rotationAnimation;
    positiveRotation = rotationAnimation;
}

void Hover::update(const float& dt)
{
    t1 += dt;

    t1 = std::fmod(t1, glm::two_pi<float>());

    // Translate
    float translationFactor = 0.5f * std::sinf(t1 - glm::half_pi<float>()) + 0.5f;

    glm::vec3 position = glm::mix(beginPos, endPos, translationFactor);

    // Rotate
    t2 += dt;

    t2 = std::fmod(t2, glm::two_pi<float>() / rotationSpeedFactor);

    float rotationFactor = 0.5f * std::sinf(t2 * rotationSpeedFactor - glm::half_pi<float>()) + 0.5f;

    glm::vec3 rotation = glm::mix(positiveRotation, negativeRotation, rotationFactor);

    // Apply transformations
    Transform* transform = host->getTransform();

    transform->setPosition(position);

    transform->setRotationQuat(beginRotation);

    transform->rotate(rotation.x, rotation.y, rotation.z);
}

void Hover::reset()
{
    Transform* transform = host->getTransform();

    transform->setPosition(beginPos);
    transform->setRotationQuat(beginRotation);

    t1 = 0.0f;
    t2 = glm::pi<float>();
}
