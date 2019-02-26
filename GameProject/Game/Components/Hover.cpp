#include "Hover.h"

#include <Engine/Entity/Entity.h>
#include <glm/gtc/random.hpp>

Hover::Hover(Entity* host)
    :Component(host, "Hover"),
    t(0.0f)
{
    Transform* transform = host->getTransform();

    beginPos = transform->getPosition();
    endPos = beginPos + translationAnimation;

    // The beginning rotation is used when resetting the hover animation
    beginRotation = transform->getRotationQuat();

    // eulerAngles returns pitch, yaw, roll: change the order to yaw, pitch, roll
    glm::vec3 eulers = glm::eulerAngles(beginRotation);

    float temp = eulers.x;

    eulers.x = eulers.y;
    eulers.y = temp;

    negativeRotation = eulers - rotationAnimation;
    positiveRotation = eulers + rotationAnimation;
}

void Hover::update(const float& dt)
{
    t += dt;

    t = std::fmod(t, glm::two_pi<float>());

    // Translate
    float translationFactor = 0.5f * std::sinf(t - glm::half_pi<float>()) + 0.5f;

    //glm::vec3 translation = translationFactor * maxTranslation * dt;

    glm::vec3 position = glm::mix(beginPos, endPos, translationFactor);

    // Rotate
    //float rotationFactor = 0.5f * sinf(t + glm::half_pi<float>()) + 0.5f;

    glm::vec3 rotation = glm::mix(positiveRotation, negativeRotation, translationFactor);

    // Apply transformations
    Transform* transform = host->getTransform();

    /*transform->translate(translation);
    transform->rotate(rotation.x, rotation.y, rotation.z);*/

    transform->setPosition(position);
    transform->setRotation(rotation);

    // Store total amount of relative transformations applied by Hover
    //totalTranslation += translation;
    //totalRotation += rotation;
}

void Hover::reset()
{
    Transform* transform = host->getTransform();

    transform->setPosition(beginPos);
    transform->setRotationQuat(beginRotation);

/*    transform->translate(-totalTranslation);
    transform->rotate(-totalRotation.x, -totalRotation.y, -totalRotation.z);

    totalTranslation = {0.0f, 0.0f, 0.0f};
    totalRotation = {0.0f, 0.0f, 0.0f};*/

    t = 0.0f;
}
