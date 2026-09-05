#include "DayNightSystem.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

namespace
{
    float GetTimeDistance(
        float a,
        float b
    )
    {
        float distance =
            std::fabs(
                a - b
            );

        if (distance > 12.0f)
        {
            distance =
                24.0f - distance;
        }

        return distance;
    }
}

void DayNightSystem::Update(
    float deltaTime
)
{
    if (enabled)
    {
        timeOfDay +=
            deltaTime *
            cycleSpeed;

        if (timeOfDay >= 24.0f)
        {
            timeOfDay -=
                24.0f;
        }

        if (timeOfDay < 0.0f)
        {
            timeOfDay +=
                24.0f;
        }
    }

    const float pi =
        3.14159265f;

    float sunAngle =
        (
            timeOfDay /
            24.0f
            ) *
        2.0f *
        pi -
        pi * 0.5f;

    float sunHeight =
        std::sin(
            sunAngle
        );

    float daylight =
        glm::clamp(
            (
                sunHeight +
                0.15f
                ) /
            1.15f,
            0.0f,
            1.0f
        );

    float sunriseBlend =
        glm::clamp(
            1.0f -
            GetTimeDistance(
                timeOfDay,
                6.2f
            ) /
            1.5f,
            0.0f,
            1.0f
        );

    float sunsetBlend =
        glm::clamp(
            1.0f -
            GetTimeDistance(
                timeOfDay,
                18.2f
            ) /
            1.5f,
            0.0f,
            1.0f
        );

    float warmBlend =
        glm::max(
            sunriseBlend,
            sunsetBlend
        );

    glm::vec3 daySunColor =
        glm::vec3(
            1.10f,
            1.05f,
            0.95f
        );

    glm::vec3 sunsetSunColor =
        glm::vec3(
            1.35f,
            0.75f,
            0.35f
        );

    glm::vec3 nightSunColor =
        glm::vec3(
            0.18f,
            0.22f,
            0.38f
        );

    glm::vec3 daySkyColor =
        glm::vec3(
            0.38f,
            0.55f,
            0.78f
        );

    glm::vec3 sunsetSkyColor =
        glm::vec3(
            0.80f,
            0.38f,
            0.18f
        );

    glm::vec3 nightSkyColor =
        glm::vec3(
            0.025f,
            0.030f,
            0.060f
        );

    sunColor =
        glm::mix(
            nightSunColor,
            daySunColor,
            daylight
        );

    sunColor =
        glm::mix(
            sunColor,
            sunsetSunColor,
            warmBlend
        );

    skyColor =
        glm::mix(
            nightSkyColor,
            daySkyColor,
            daylight
        );

    skyColor =
        glm::mix(
            skyColor,
            sunsetSkyColor,
            warmBlend
        );

    float sunY =
        -glm::max(
            sunHeight,
            0.12f
        );

    sunDirection =
        glm::normalize(
            glm::vec3(
                std::cos(
                    sunAngle
                ) * 0.55f,
                sunY,
                -0.35f
            )
        );
}

void DayNightSystem::SetGoldenHour()
{
    timeOfDay =
        18.0f;

    enabled =
        false;
}

void DayNightSystem::SetNight()
{
    timeOfDay =
        22.0f;

    enabled =
        false;
}

void DayNightSystem::SetDay()
{
    timeOfDay =
        14.0f;

    enabled =
        false;
}