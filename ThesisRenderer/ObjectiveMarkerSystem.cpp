#include "ObjectiveMarkerSystem.h"

#include "SceneObject.h"

#include <GLFW/glfw3.h>

#include <cmath>
#include <cstdio>
#include <string>

namespace
{
    bool IsCoinObjectLocal(
        SceneObject* object
    )
    {
        if (object == nullptr)
            return false;

        if (object->editorGameplayType == "Coin")
            return true;

        if (object->name.find("Coin") != std::string::npos)
            return true;

        return false;
    }

    bool IsTriggerZoneObjectLocal(
        SceneObject* object
    )
    {
        if (object == nullptr)
            return false;

        if (object->editorGameplayType == "TriggerZone")
            return true;

        if (object->name.find("Trigger Zone") != std::string::npos)
            return true;

        return false;
    }

    bool IsMonsterSpawnObjectLocal(
        SceneObject* object
    )
    {
        if (object == nullptr)
            return false;

        if (object->editorGameplayType == "MonsterSpawn")
            return true;

        if (object->name.find("Monster Spawn") != std::string::npos)
            return true;

        return false;
    }

    bool IsMusicGateObjectLocal(
        SceneObject* object
    )
    {
        if (object == nullptr)
            return false;

        if (object->editorGameplayType == "MusicGate")
            return true;

        if (object->name == "Music Gate")
            return true;

        return false;
    }

    bool IsMusicNpcObjectLocal(
        SceneObject* object
    )
    {
        if (object == nullptr)
            return false;

        if (object->editorGameplayType == "MusicNPC")
            return true;

        if (object->name.find("Music NPC") != std::string::npos)
            return true;

        return false;
    }
}

bool ObjectiveMarkerSystem::WorldToScreenPosition(
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

void ObjectiveMarkerSystem::DrawSingleMarker(
    const char* label,
    const glm::vec3& worldPosition,
    const glm::vec3& cameraPosition,
    const glm::mat4& view,
    const glm::mat4& projection,
    int screenWidth,
    int screenHeight,
    ImU32 color,
    MarkerShape shape
)
{
    ImVec2 screenPosition;

    if (
        !WorldToScreenPosition(
            worldPosition,
            view,
            projection,
            screenWidth,
            screenHeight,
            screenPosition
        )
        )
    {
        return;
    }

    float distance =
        glm::length(
            worldPosition -
            cameraPosition
        );

    if (distance > 140.0f)
        return;

    ImDrawList* drawList =
        ImGui::GetForegroundDrawList();

    float pulse =
        1.0f +
        std::sin(
            static_cast<float>(
                glfwGetTime()
                ) *
            4.0f +
            worldPosition.x *
            0.10f
        ) *
        0.18f;

    float radius =
        8.0f *
        pulse;

    float outlineRadius =
        radius +
        4.0f;

    if (shape == MarkerShape::Circle)
    {
        drawList->AddCircleFilled(
            screenPosition,
            radius,
            color,
            28
        );

        drawList->AddCircle(
            screenPosition,
            outlineRadius,
            IM_COL32(255, 255, 255, 220),
            28,
            2.0f
        );
    }
    else if (shape == MarkerShape::Diamond)
    {
        ImVec2 top(screenPosition.x, screenPosition.y - outlineRadius);
        ImVec2 right(screenPosition.x + outlineRadius, screenPosition.y);
        ImVec2 bottom(screenPosition.x, screenPosition.y + outlineRadius);
        ImVec2 left(screenPosition.x - outlineRadius, screenPosition.y);

        drawList->AddQuadFilled(
            top,
            right,
            bottom,
            left,
            color
        );

        drawList->AddQuad(
            top,
            right,
            bottom,
            left,
            IM_COL32(255, 255, 255, 230),
            2.0f
        );
    }
    else if (shape == MarkerShape::Triangle)
    {
        ImVec2 top(screenPosition.x, screenPosition.y - outlineRadius);
        ImVec2 left(screenPosition.x - outlineRadius, screenPosition.y + outlineRadius);
        ImVec2 right(screenPosition.x + outlineRadius, screenPosition.y + outlineRadius);

        drawList->AddTriangleFilled(
            top,
            left,
            right,
            color
        );

        drawList->AddTriangle(
            top,
            left,
            right,
            IM_COL32(255, 255, 255, 230),
            2.0f
        );
    }
    else if (shape == MarkerShape::Square)
    {
        ImVec2 minPoint(
            screenPosition.x - outlineRadius,
            screenPosition.y - outlineRadius
        );

        ImVec2 maxPoint(
            screenPosition.x + outlineRadius,
            screenPosition.y + outlineRadius
        );

        drawList->AddRectFilled(
            minPoint,
            maxPoint,
            color,
            4.0f
        );

        drawList->AddRect(
            minPoint,
            maxPoint,
            IM_COL32(255, 255, 255, 230),
            4.0f,
            0,
            2.0f
        );
    }
    else if (shape == MarkerShape::Ring)
    {
        drawList->AddCircle(
            screenPosition,
            outlineRadius,
            color,
            32,
            4.0f
        );

        drawList->AddCircleFilled(
            screenPosition,
            radius * 0.45f,
            IM_COL32(255, 255, 255, 230),
            24
        );
    }

    char textBuffer[128];

    std::snprintf(
        textBuffer,
        sizeof(textBuffer),
        "%s %.0fm",
        label,
        distance
    );

    ImVec2 textSize =
        ImGui::CalcTextSize(
            textBuffer
        );

    ImVec2 textPosition(
        screenPosition.x - textSize.x * 0.5f,
        screenPosition.y - 38.0f
    );

    drawList->AddRectFilled(
        ImVec2(textPosition.x - 6.0f, textPosition.y - 3.0f),
        ImVec2(textPosition.x + textSize.x + 6.0f, textPosition.y + textSize.y + 3.0f),
        IM_COL32(0, 0, 0, 165),
        6.0f
    );

    drawList->AddText(
        textPosition,
        IM_COL32(255, 255, 255, 255),
        textBuffer
    );
}

void ObjectiveMarkerSystem::DrawMarkers(
    Scene& scene,
    const glm::vec3& cameraPosition,
    const glm::mat4& view,
    const glm::mat4& projection,
    int screenWidth,
    int screenHeight
)
{
    for (SceneObject* object : scene.objects)
    {
        if (object == nullptr)
            continue;

        if (!object->visible)
            continue;

        glm::vec3 markerPosition =
            object->transform.position +
            glm::vec3(
                0.0f,
                2.2f,
                0.0f
            );

        if (IsCoinObjectLocal(object))
        {
            DrawSingleMarker(
                "COIN",
                markerPosition,
                cameraPosition,
                view,
                projection,
                screenWidth,
                screenHeight,
                IM_COL32(255, 215, 0, 255),
                MarkerShape::Circle
            );
        }
        else if (IsTriggerZoneObjectLocal(object))
        {
            DrawSingleMarker(
                "TRIGGER",
                markerPosition,
                cameraPosition,
                view,
                projection,
                screenWidth,
                screenHeight,
                IM_COL32(255, 90, 90, 255),
                MarkerShape::Diamond
            );
        }
        else if (IsMonsterSpawnObjectLocal(object))
        {
            DrawSingleMarker(
                "MONSTER",
                markerPosition,
                cameraPosition,
                view,
                projection,
                screenWidth,
                screenHeight,
                IM_COL32(180, 60, 255, 255),
                MarkerShape::Triangle
            );
        }
        else if (IsMusicGateObjectLocal(object))
        {
            DrawSingleMarker(
                "GATE",
                markerPosition,
                cameraPosition,
                view,
                projection,
                screenWidth,
                screenHeight,
                IM_COL32(80, 180, 255, 255),
                MarkerShape::Square
            );
        }
        else if (IsMusicNpcObjectLocal(object))
        {
            DrawSingleMarker(
                "NPC",
                markerPosition,
                cameraPosition,
                view,
                projection,
                screenWidth,
                screenHeight,
                IM_COL32(120, 255, 160, 255),
                MarkerShape::Ring
            );
        }
    }
}

void ObjectiveMarkerSystem::DrawHeader(
    Scene& scene,
    int screenWidth
)
{
    int visibleCoins =
        0;

    bool hasTrigger =
        false;

    bool hasMonster =
        false;

    bool hasGate =
        false;

    bool hasNpc =
        false;

    for (SceneObject* object : scene.objects)
    {
        if (object == nullptr)
            continue;

        if (!object->visible)
            continue;

        if (IsCoinObjectLocal(object))
            visibleCoins++;

        if (IsTriggerZoneObjectLocal(object))
            hasTrigger = true;

        if (IsMonsterSpawnObjectLocal(object))
            hasMonster = true;

        if (IsMusicGateObjectLocal(object))
            hasGate = true;

        if (IsMusicNpcObjectLocal(object))
            hasNpc = true;
    }

    std::string objectiveText =
        "Objective: Explore the scene";

    if (visibleCoins > 0)
    {
        objectiveText =
            "Objective: Collect coins  |  Remaining: " +
            std::to_string(
                visibleCoins
            );
    }
    else if (
        hasGate &&
        hasNpc
        )
    {
        objectiveText =
            "Objective: Reach the Music Gate and rescue the NPC";
    }
    else if (
        hasTrigger &&
        hasMonster
        )
    {
        objectiveText =
            "Objective: Enter the trigger zone and survive the monster event";
    }

    ImDrawList* drawList =
        ImGui::GetForegroundDrawList();

    ImVec2 textSize =
        ImGui::CalcTextSize(
            objectiveText.c_str()
        );

    ImVec2 textPosition(
        static_cast<float>(screenWidth) * 0.5f -
        textSize.x * 0.5f,
        22.0f
    );

    drawList->AddRectFilled(
        ImVec2(textPosition.x - 14.0f, textPosition.y - 7.0f),
        ImVec2(textPosition.x + textSize.x + 14.0f, textPosition.y + textSize.y + 7.0f),
        IM_COL32(0, 0, 0, 170),
        8.0f
    );

    drawList->AddText(
        textPosition,
        IM_COL32(255, 255, 255, 255),
        objectiveText.c_str()
    );
}

void ObjectiveMarkerSystem::Draw(
    Scene& scene,
    const glm::vec3& cameraPosition,
    const glm::mat4& view,
    const glm::mat4& projection,
    int screenWidth,
    int screenHeight
)
{
    if (!enabled)
        return;

    DrawMarkers(
        scene,
        cameraPosition,
        view,
        projection,
        screenWidth,
        screenHeight
    );

    DrawHeader(
        scene,
        screenWidth
    );
}