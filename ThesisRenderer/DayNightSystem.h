#pragma once

#include <glm/glm.hpp>

class DayNightSystem
{
public:
    bool enabled =
        false;

    float timeOfDay =
        14.0f;

    float cycleSpeed =
        0.25f;

    glm::vec3 sunDirection =
        glm::vec3(
            -0.55f,
            -1.0f,
            -0.35f
        );

    glm::vec3 sunColor =
        glm::vec3(
            1.10f,
            1.05f,
            0.95f
        );

    glm::vec3 skyColor =
        glm::vec3(
            0.10f,
            0.12f,
            0.16f
        );

public:
    void Update(
        float deltaTime
    );

    void SetGoldenHour();

    void SetNight();

    void SetDay();
};