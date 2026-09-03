#include "GameplayFeedbackFX.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <cmath>
namespace
{
    float ClampFloat(
        float value,
        float minValue,
        float maxValue
    )
    {
        if (value < minValue)
            return minValue;

        if (value > maxValue)
            return maxValue;

        return value;
    }
}
void GameplayFeedbackFX::AddFloatingText(
    const glm::vec3& worldPosition,
    const std::string& text,
    const ImVec4& color
)
{
    FloatingText floatingText;

    floatingText.worldPosition =
        worldPosition;

    floatingText.text =
        text;

    floatingText.lifetime =
        1.25f;

    floatingText.duration =
        1.25f;

    floatingText.color =
        color;

    floatingTexts.push_back(
        floatingText
    );
}

void GameplayFeedbackFX::ShowNotification(
    const std::string& text,
    const ImVec4& color,
    float duration
)
{
    notificationText =
        text;

    notificationColor =
        color;

    notificationDuration =
        duration;

    notificationTimer =
        duration;
}

void GameplayFeedbackFX::Update(
    float deltaTime
)
{
    for (FloatingText& text : floatingTexts)
    {
        text.lifetime -=
            deltaTime;
    }

    floatingTexts.erase(
        std::remove_if(
            floatingTexts.begin(),
            floatingTexts.end(),
            [](const FloatingText& text)
            {
                return text.lifetime <= 0.0f;
            }
        ),
        floatingTexts.end()
    );

    if (notificationTimer > 0.0f)
    {
        notificationTimer -=
            deltaTime;

        if (notificationTimer < 0.0f)
        {
            notificationTimer =
                0.0f;
        }
    }
}

bool GameplayFeedbackFX::WorldToScreenPosition(
    const glm::vec3& worldPosition,
    const glm::mat4& view,
    const glm::mat4& projection,
    int screenWidth,
    int screenHeight,
    ImVec2& screenPosition
) const
{
    glm::vec4 clipPosition =
        projection *
        view *
        glm::vec4(
            worldPosition,
            1.0f
        );

    if (clipPosition.w <= 0.01f)
        return false;

    glm::vec3 ndcPosition =
        glm::vec3(
            clipPosition
        ) /
        clipPosition.w;

    if (
        ndcPosition.x < -1.0f ||
        ndcPosition.x > 1.0f ||
        ndcPosition.y < -1.0f ||
        ndcPosition.y > 1.0f
        )
    {
        return false;
    }

    screenPosition.x =
        (
            ndcPosition.x * 0.5f +
            0.5f
            ) *
        static_cast<float>(screenWidth);

    screenPosition.y =
        (
            1.0f -
            (
                ndcPosition.y * 0.5f +
                0.5f
                )
            ) *
        static_cast<float>(screenHeight);

    return true;
}

void GameplayFeedbackFX::DrawFloatingTexts(
    const glm::mat4& view,
    const glm::mat4& projection,
    int screenWidth,
    int screenHeight
)
{
    ImDrawList* drawList =
        ImGui::GetForegroundDrawList();

    for (const FloatingText& text : floatingTexts)
    {
        ImVec2 screenPosition;

        if (
            !WorldToScreenPosition(
                text.worldPosition,
                view,
                projection,
                screenWidth,
                screenHeight,
                screenPosition
            )
            )
        {
            continue;
        }

        float progress =
            1.0f -
            text.lifetime /
            text.duration;

        float alpha =
            ClampFloat(
                text.lifetime /
                text.duration,
                0.0f,
                1.0f
            );

        screenPosition.y -=
            progress *
            48.0f;

        ImVec4 finalColor =
            text.color;

        finalColor.w *=
            alpha;

        ImU32 textColor =
            ImGui::ColorConvertFloat4ToU32(
                finalColor
            );

        ImVec2 textSize =
            ImGui::CalcTextSize(
                text.text.c_str()
            );

        ImVec2 textPosition =
            ImVec2(
                screenPosition.x -
                textSize.x * 0.5f,
                screenPosition.y -
                textSize.y * 0.5f
            );

        drawList->AddRectFilled(
            ImVec2(
                textPosition.x - 8.0f,
                textPosition.y - 4.0f
            ),
            ImVec2(
                textPosition.x + textSize.x + 8.0f,
                textPosition.y + textSize.y + 4.0f
            ),
            IM_COL32(
                0,
                0,
                0,
                static_cast<int>(
                    150.0f *
                    alpha
                    )
            ),
            6.0f
        );

        drawList->AddText(
            textPosition,
            textColor,
            text.text.c_str()
        );
    }
}

void GameplayFeedbackFX::DrawNotification(
    int screenWidth
)
{
    if (notificationTimer <= 0.0f)
        return;

    float alpha =
        ClampFloat(
            notificationTimer /
            notificationDuration,
            0.0f,
            1.0f
        );

    ImDrawList* drawList =
        ImGui::GetForegroundDrawList();

    ImVec2 textSize =
        ImGui::CalcTextSize(
            notificationText.c_str()
        );

    ImVec2 textPosition =
        ImVec2(
            static_cast<float>(screenWidth) * 0.5f -
            textSize.x * 0.5f,
            72.0f
        );

    drawList->AddRectFilled(
        ImVec2(
            textPosition.x - 16.0f,
            textPosition.y - 8.0f
        ),
        ImVec2(
            textPosition.x + textSize.x + 16.0f,
            textPosition.y + textSize.y + 8.0f
        ),
        IM_COL32(
            0,
            0,
            0,
            static_cast<int>(
                180.0f *
                alpha
                )
        ),
        8.0f
    );

    ImVec4 finalColor =
        notificationColor;

    finalColor.w *=
        alpha;

    drawList->AddText(
        textPosition,
        ImGui::ColorConvertFloat4ToU32(
            finalColor
        ),
        notificationText.c_str()
    );
}

void GameplayFeedbackFX::DrawMonsterDangerOverlay(
    int screenWidth,
    int screenHeight,
    bool monsterEventActive,
    bool monsterPlayerCaught,
    bool musicRescueWin
)
{
    if (
        !monsterEventActive ||
        monsterPlayerCaught ||
        musicRescueWin
        )
    {
        return;
    }

    ImDrawList* drawList =
        ImGui::GetForegroundDrawList();

    float pulse =
        0.5f +
        0.5f *
        std::sin(
            static_cast<float>(
                glfwGetTime()
                ) *
            7.0f
        );

    int alpha =
        static_cast<int>(
            28.0f +
            pulse *
            32.0f
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
            160,
            0,
            0,
            alpha
        )
    );

    float borderThickness =
        8.0f +
        pulse *
        4.0f;

    drawList->AddRect(
        ImVec2(
            6.0f,
            6.0f
        ),
        ImVec2(
            static_cast<float>(screenWidth) - 6.0f,
            static_cast<float>(screenHeight) - 6.0f
        ),
        IM_COL32(
            255,
            40,
            40,
            180
        ),
        0.0f,
        0,
        borderThickness
    );

    const char* dangerText =
        "DANGER: MONSTER CHASING";

    ImVec2 textSize =
        ImGui::CalcTextSize(
            dangerText
        );

    ImVec2 textPosition =
        ImVec2(
            static_cast<float>(screenWidth) * 0.5f -
            textSize.x * 0.5f,
            112.0f
        );

    drawList->AddText(
        textPosition,
        IM_COL32(
            255,
            80,
            80,
            255
        ),
        dangerText
    );
}

void GameplayFeedbackFX::Draw(
    const glm::mat4& view,
    const glm::mat4& projection,
    int screenWidth,
    int screenHeight,
    bool monsterEventActive,
    bool monsterPlayerCaught,
    bool musicRescueWin
)
{
    if (!enabled)
        return;

    DrawFloatingTexts(
        view,
        projection,
        screenWidth,
        screenHeight
    );

    DrawNotification(
        screenWidth
    );

    DrawMonsterDangerOverlay(
        screenWidth,
        screenHeight,
        monsterEventActive,
        monsterPlayerCaught,
        musicRescueWin
    );
}