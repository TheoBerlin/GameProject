#include "Hover.h"

#include <Engine/Entity/Entity.h>

Hover::Hover(Entity* host)
    :Component(host, "Hover"),
    t(0.0f)
{
}

void Hover::update(const float& dt)
{
    t += dt;

    t = std::fmod(t, glm::two_pi<float>());

    float animationFactor = std::sinf(t);

    translation = animationFactor * maxTranslation * dt;

    rotation = animationFactor * maxRotation * dt;

    Transform* transform = host->getTransform();

    transform->translate(translation);
    transform->rotate(rotation.x, rotation.y, rotation.z);
}
