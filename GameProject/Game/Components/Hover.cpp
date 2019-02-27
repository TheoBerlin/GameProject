#include "Hover.h"

#include <Engine/Entity/Entity.h>
#include <glm/gtc/random.hpp>

Hover::Hover(Entity* host)
    :Component(host, "Hover"),
    totalTranslation({0.0f, 0.0f, 0.0f}),
    totalRotation({0.0f, 0.0f, 0.0f})
{
    // Randomize starting animation time
    startT = glm::linearRand(0.0f, glm::two_pi<float>());

    t = startT;
}

void Hover::update(const float& dt)
{
    t += dt;

    t = std::fmod(t, glm::two_pi<float>());

    // Translate
    float translationFactor = 0.5f * sinf(t - glm::half_pi<float>()) + 0.5f;

    glm::vec3 translation = translationFactor * maxTranslation;

    // Rotate
    float rotationFactor = 0.5f * sinf(t) + 0.5f;

    glm::vec3 rotation = glm::mix(maxRotation, -maxRotation, rotationFactor);

    // Apply transformations
    Transform* transform = host->getTransform();

    transform->translate(-totalTranslation);
    transform->translate(translation);

    transform->rotate(-totalRotation.x, -totalRotation.y, -totalRotation.z);
    transform->rotate(rotation.x, rotation.y, rotation.z);

    // Store total amount of relative transformations applied by Hover
    totalTranslation = translation;
    totalRotation = rotation;
}

void Hover::reset()
{
    Transform* transform = host->getTransform();

    transform->translate(-totalTranslation);
    transform->rotate(-totalRotation.x, -totalRotation.y, -totalRotation.z);

    totalTranslation = {0.0f, 0.0f, 0.0f};
    totalRotation = {0.0f, 0.0f, 0.0f};

    t = startT;
}
