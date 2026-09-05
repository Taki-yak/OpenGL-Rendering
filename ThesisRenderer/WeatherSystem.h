#pragma once

#include <glm/glm.hpp>

class WeatherSystem
{
public:
    static const int PresetCount =
        4;

    static const char* const PresetNames[
        PresetCount
    ];

public:
    bool enabled =
        true;

    bool showOverlay =
        true;

    int presetIndex =
        0;

    bool useFog =
        false;

    glm::vec3 fogColor =
        glm::vec3(
            0.55f,
            0.62f,
            0.68f
        );

    float fogStart =
        60.0f;

    float fogEnd =
        190.0f;

    float fogStrength =
        0.0f;

    bool rainOverlay =
        false;

    bool stormOverlay =
        false;

    float rainIntensity =
        0.0f;

public:
    void ApplyToAtmosphere(
        glm::vec3& skyColor,
        glm::vec3& sunColor
    );

    void DrawOverlay(
        int screenWidth,
        int screenHeight
    ) const;
};