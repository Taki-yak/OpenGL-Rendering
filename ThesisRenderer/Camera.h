#pragma once

#include <glm/glm.hpp>

class Camera
{
public:

    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;

    float Yaw;
    float Pitch;

    Camera();

    glm::mat4 GetViewMatrix();

    void ProcessMouse(float xoffset, float yoffset);
};