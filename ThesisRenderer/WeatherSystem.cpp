#include "WeatherSystem.h"

#include "imgui.h"

#include <GLFW/glfw3.h>

#include <cmath>

const char* const WeatherSystem::PresetNames[
    WeatherSystem::PresetCount
] =
    {
        "Clear",
        "Foggy Forest",
        "Light Rain",
        "Storm"
    };

    void WeatherSystem::ApplyToAtmosphere(
        glm::vec3& skyColor,
        glm::vec3& sunColor
    )
    {
        useFog =
            false;

        rainOverlay =
            false;

        stormOverlay =
            false;

        rainIntensity =
            0.0f;

        fogColor =
            glm::vec3(
                0.55f,
                0.62f,
                0.68f
            );

        fogStart =
            65.0f;

        fogEnd =
            190.0f;

        fogStrength =
            0.0f;

        if (!enabled)
            return;

        // ================= CLEAR =================
        if (presetIndex == 0)
        {
            return;
        }

        // ================= FOGGY FOREST =================
        if (presetIndex == 1)
        {
            useFog =
                true;

            fogColor =
                glm::vec3(
                    0.48f,
                    0.55f,
                    0.52f
                );

            fogStart =
                28.0f;

            fogEnd =
                145.0f;

            fogStrength =
                0.95f;

            skyColor =
                glm::mix(
                    skyColor,
                    fogColor,
                    0.42f
                );

            sunColor *=
                0.72f;
        }

        // ================= LIGHT RAIN =================
        else if (presetIndex == 2)
        {
            useFog =
                true;

            rainOverlay =
                true;

            rainIntensity =
                0.45f;

            fogColor =
                glm::vec3(
                    0.30f,
                    0.36f,
                    0.44f
                );

            fogStart =
                35.0f;

            fogEnd =
                135.0f;

            fogStrength =
                0.80f;

            skyColor =
                glm::mix(
                    skyColor,
                    glm::vec3(
                        0.13f,
                        0.16f,
                        0.21f
                    ),
                    0.55f
                );

            sunColor *=
                0.52f;
        }

        // ================= STORM =================
        else if (presetIndex == 3)
        {
            useFog =
                true;

            rainOverlay =
                true;

            stormOverlay =
                true;

            rainIntensity =
                0.90f;

            fogColor =
                glm::vec3(
                    0.14f,
                    0.16f,
                    0.20f
                );

            fogStart =
                22.0f;

            fogEnd =
                115.0f;

            fogStrength =
                1.15f;

            skyColor =
                glm::mix(
                    skyColor,
                    glm::vec3(
                        0.055f,
                        0.065f,
                        0.085f
                    ),
                    0.78f
                );

            sunColor *=
                0.32f;
        }
    }

    void WeatherSystem::DrawOverlay(
        int screenWidth,
        int screenHeight
    ) const
    {
        if (!enabled)
            return;

        if (!showOverlay)
            return;

        if (
            !rainOverlay &&
            !stormOverlay
            )
        {
            return;
        }

        ImDrawList* drawList =
            ImGui::GetForegroundDrawList();

        float time =
            static_cast<float>(
                glfwGetTime()
                );

        if (stormOverlay)
        {
            float stormPulse =
                0.5f +
                0.5f *
                std::sin(
                    time *
                    2.2f
                );

            drawList->AddRectFilled(
                ImVec2(
                    0.0f,
                    0.0f
                ),
                ImVec2(
                    static_cast<float>(screenWidth),
                    static_cast<float>(screenHeight)
                ),
                IM_COL32(
                    10,
                    15,
                    25,
                    static_cast<int>(
                        35.0f +
                        stormPulse *
                        25.0f
                        )
                )
            );
        }

        int rainLineCount =
            static_cast<int>(
                160.0f *
                rainIntensity
                );

        for (int i = 0; i < rainLineCount; i++)
        {
            float seed =
                static_cast<float>(
                    i
                    );

            float x =
                std::fmod(
                    seed * 73.13f +
                    time * 420.0f,
                    static_cast<float>(screenWidth) + 160.0f
                ) -
                80.0f;

            float y =
                std::fmod(
                    seed * 151.71f +
                    time * 650.0f,
                    static_cast<float>(screenHeight) + 160.0f
                ) -
                80.0f;

            float length =
                22.0f +
                rainIntensity *
                24.0f;

            drawList->AddLine(
                ImVec2(
                    x,
                    y
                ),
                ImVec2(
                    x - 16.0f,
                    y + length
                ),
                IM_COL32(
                    170,
                    190,
                    230,
                    static_cast<int>(
                        85.0f +
                        rainIntensity *
                        75.0f
                        )
                ),
                1.2f
            );
        }

        if (stormOverlay)
        {
            float lightning =
                std::sin(
                    time *
                    3.7f
                ) *
                std::sin(
                    time *
                    8.1f
                );

            if (lightning > 0.92f)
            {
                drawList->AddRectFilled(
                    ImVec2(
                        0.0f,
                        0.0f
                    ),
                    ImVec2(
                        static_cast<float>(screenWidth),
                        static_cast<float>(screenHeight)
                    ),
                    IM_COL32(
                        220,
                        235,
                        255,
                        38
                    )
                );
            }
        }
    }