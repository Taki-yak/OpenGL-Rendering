#include "CinematicOverlay.h"

#include "imgui.h"

#include <GLFW/glfw3.h>

void CinematicOverlay::HandleShortcut(
    GLFWwindow* window
)
{
    if (window == nullptr)
        return;

    bool f10Down =
        glfwGetKey(
            window,
            GLFW_KEY_F10
        ) == GLFW_PRESS;

    if (
        f10Down &&
        !togglePressed
        )
    {
        enabled =
            !enabled;
    }

    togglePressed =
        f10Down;
}

void CinematicOverlay::Draw(
    int screenWidth,
    int screenHeight
) const
{
    if (!enabled)
        return;

    ImDrawList* drawList =
        ImGui::GetForegroundDrawList();

    drawList->AddRectFilled(
        ImVec2(
            0.0f,
            0.0f
        ),
        ImVec2(
            static_cast<float>(screenWidth),
            blackBarHeight
        ),
        IM_COL32(
            0,
            0,
            0,
            255
        )
    );

    drawList->AddRectFilled(
        ImVec2(
            0.0f,
            static_cast<float>(screenHeight) -
            blackBarHeight
        ),
        ImVec2(
            static_cast<float>(screenWidth),
            static_cast<float>(screenHeight)
        ),
        IM_COL32(
            0,
            0,
            0,
            255
        )
    );

    const char* title =
        "ORION ENGINE";

    const char* subtitle =
        "Cinematic Presentation Mode";

    ImVec2 titleSize =
        ImGui::CalcTextSize(
            title
        );

    ImVec2 subtitleSize =
        ImGui::CalcTextSize(
            subtitle
        );

    drawList->AddText(
        ImVec2(
            static_cast<float>(screenWidth) * 0.5f -
            titleSize.x * 0.5f,
            24.0f
        ),
        IM_COL32(
            255,
            255,
            255,
            230
        ),
        title
    );

    drawList->AddText(
        ImVec2(
            static_cast<float>(screenWidth) * 0.5f -
            subtitleSize.x * 0.5f,
            static_cast<float>(screenHeight) -
            blackBarHeight +
            24.0f
        ),
        IM_COL32(
            200,
            200,
            200,
            220
        ),
        subtitle
    );
}