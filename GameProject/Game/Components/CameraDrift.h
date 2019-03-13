#pragma once

#include <Engine/Components/Camera.h>
#include <Engine/Components/Component.h>

/*
    Slowly aligns a camera's forward with its host entity's forward
    If the camera has an offset, CameraDrift keeps the camera at the desired distance
    and keeps the Camera looking at the entity.
*/
class CameraDrift : public Component
{
public:
    CameraDrift(Entity* host);
    ~CameraDrift();

    void update(const float& dt);

private:
    // Percentage of forward angle being nullified per second
    const float angleCorrectionFactor = 3.5f;
    const float maxAngle = glm::half_pi<float>();

    Camera* camera;
};
