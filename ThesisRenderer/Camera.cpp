#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
    Position = glm::vec3(0.0f, 0.0f, 3.0f);
    Front = glm::vec3(0.0f, 0.0f, -1.0f);
    Up = glm::vec3(0.0f, 1.0f, 0.0f);

    Yaw = -90.0f;
    Pitch = 0.0f;
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessMouse(float xoffset, float yoffset)
{
    float sensitivity = 0.1f;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (Pitch > 89.0f) Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;

    glm::vec3 direction;

    direction.x =
        cos(glm::radians(Yaw)) *
        cos(glm::radians(Pitch));

    direction.y =
        sin(glm::radians(Pitch));

    direction.z =
        sin(glm::radians(Yaw)) *
        cos(glm::radians(Pitch));

    Front = glm::normalize(direction);
}