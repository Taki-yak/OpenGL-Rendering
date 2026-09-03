#pragma once

#include "imgui.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>

class GameplayFeedbackFX
{
public:
    bool enabled =
        true;

    void AddFloatingText(
        const glm::vec3& worldPosition,
        const std::string& text,
        const ImVec4& color
    );

    void ShowNotification(
        const std::string& text,
        const ImVec4& color,
        float duration
    );

    void Update(
        float deltaTime
    );

    void Draw(
        const glm::mat4& view,
        const glm::mat4& projection,
        int screenWidth,
        int screenHeight,
        bool monsterEventActive,
        bool monsterPlayerCaught,
        bool musicRescueWin
    );

private:
    struct FloatingText
    {
        glm::vec3 worldPosition =
            glm::vec3(0.0f);

        std::string text =
            "";

        float lifetime =
            0.0f;

        float duration =
            0.0f;

        ImVec4 color =
            ImVec4(
                1.0f,
                1.0f,
                1.0f,
                1.0f
            );
    };

    std::vector<FloatingText> floatingTexts;

    std::string notificationText =
        "";

    float notificationTimer =
        0.0f;

    float notificationDuration =
        1.0f;

    ImVec4 notificationColor =
        ImVec4(
            1.0f,
            1.0f,
            1.0f,
            1.0f
        );

private:
    bool WorldToScreenPosition(
        const glm::vec3& worldPosition,
        const glm::mat4& view,
        const glm::mat4& projection,
        int screenWidth,
        int screenHeight,
        ImVec2& screenPosition
    ) const;

    void DrawFloatingTexts(
        const glm::mat4& view,
        const glm::mat4& projection,
        int screenWidth,
        int screenHeight
    );

    void DrawNotification(
        int screenWidth
    );

    void DrawMonsterDangerOverlay(
        int screenWidth,
        int screenHeight,
        bool monsterEventActive,
        bool monsterPlayerCaught,
        bool musicRescueWin
    );
};