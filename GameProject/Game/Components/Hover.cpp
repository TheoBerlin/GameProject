#include "Hover.h"

#include <Engine/Entity/Entity.h>
#include <glm/gtc/random.hpp>

Hover::Hover(Entity* host)
    :Component(host, "Hover"),
    t(0.0f)
{
}

void Hover::update(const float& dt)
{
    t += dt;

    t = std::fmod(t, glm::two_pi<float>());

    // Translate
    float translationFactor = std::sinf(t) / 2.0f;

    glm::vec3 translation = translationFactor * maxTranslation * dt;

    // Rotate
    float rotationFactor = 0.5f * sinf(t + glm::half_pi<float>()) + 0.5f;

    glm::vec3 rotation = glm::mix(maxRotation, -maxRotation, rotationFactor) * dt;

    // Apply transformations
    Transform* transform = host->getTransform();

    transform->translate(translation);
    transform->rotate(rotation.x, rotation.y, rotation.z);
}
