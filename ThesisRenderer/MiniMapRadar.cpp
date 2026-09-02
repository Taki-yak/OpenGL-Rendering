#include "MiniMapRadar.h"

#include "SceneObject.h"
#include "imgui.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <string>

namespace
{
    enum class MiniMapIconShape
    {
        Circle,
        Diamond,
        Triangle,
        Square,
        Ring
    };

    bool IsCoinObject(
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

    bool IsTriggerZoneObject(
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

    bool IsMonsterSpawnObject(
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

    bool IsMusicGateObject(
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

    bool IsMusicNpcObject(
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

    SceneObject* FindPlayerObjectLocal(
        Scene& scene
    )
    {
        for (SceneObject* object : scene.objects)
        {
            if (object == nullptr)
                continue;

            if (object->name == "Player")
                return object;
        }

        return nullptr;
    }

    void DrawMiniMapIcon(
        ImDrawList* drawList,
        const ImVec2& position,
        ImU32 color,
        MiniMapIconShape shape,
        float size
    )
    {
        if (shape == MiniMapIconShape::Circle)
        {
            drawList->AddCircleFilled(
                position,
                size,
                color,
                20
            );
        }
        else if (shape == MiniMapIconShape::Diamond)
        {
            drawList->AddQuadFilled(
                ImVec2(position.x, position.y - size),
                ImVec2(position.x + size, position.y),
                ImVec2(position.x, position.y + size),
                ImVec2(position.x - size, position.y),
                color
            );
        }
        else if (shape == MiniMapIconShape::Triangle)
        {
            drawList->AddTriangleFilled(
                ImVec2(position.x, position.y - size),
                ImVec2(position.x - size, position.y + size),
                ImVec2(position.x + size, position.y + size),
                color
            );
        }
        else if (shape == MiniMapIconShape::Square)
        {
            drawList->AddRectFilled(
                ImVec2(position.x - size, position.y - size),
                ImVec2(position.x + size, position.y + size),
                color,
                3.0f
            );
        }
        else if (shape == MiniMapIconShape::Ring)
        {
            drawList->AddCircle(
                position,
                size,
                color,
                24,
                3.0f
            );

            drawList->AddCircleFilled(
                position,
                size * 0.35f,
                color,
                12
            );
        }
    }

    ImVec2 GetMiniMapScreenPosition(
        const glm::vec3& playerPosition,
        const glm::vec3& objectPosition,
        const ImVec2& mapCenter,
        float mapRadius,
        float mapRange
    )
    {
        if (mapRange <= 0.001f)
        {
            mapRange =
                1.0f;
        }

        glm::vec2 delta =
            glm::vec2(
                objectPosition.x - playerPosition.x,
                objectPosition.z - playerPosition.z
            );

        float distance =
            glm::length(
                delta
            );

        if (
            distance > mapRange &&
            distance > 0.001f
            )
        {
            delta =
                glm::normalize(
                    delta
                ) *
                mapRange;
        }

        float radarX =
            delta.x /
            mapRange *
            mapRadius;

        float radarY =
            delta.y /
            mapRange *
            mapRadius;

        return ImVec2(
            mapCenter.x + radarX,
            mapCenter.y - radarY
        );
    }

    void DrawMiniMapPlayerArrow(
        ImDrawList* drawList,
        const ImVec2& center,
        float rotationY
    )
    {
        float angle =
            glm::radians(
                rotationY
            );

        ImVec2 forward =
            ImVec2(
                std::sin(angle),
                -std::cos(angle)
            );

        ImVec2 right =
            ImVec2(
                forward.y,
                -forward.x
            );

        float size =
            13.0f;

        ImVec2 tip =
            ImVec2(
                center.x + forward.x * size,
                center.y + forward.y * size
            );

        ImVec2 left =
            ImVec2(
                center.x - forward.x * size * 0.65f + right.x * size * 0.65f,
                center.y - forward.y * size * 0.65f + right.y * size * 0.65f
            );

        ImVec2 rightPoint =
            ImVec2(
                center.x - forward.x * size * 0.65f - right.x * size * 0.65f,
                center.y - forward.y * size * 0.65f - right.y * size * 0.65f
            );

        drawList->AddTriangleFilled(
            tip,
            left,
            rightPoint,
            IM_COL32(
                255,
                255,
                255,
                255
            )
        );

        drawList->AddTriangle(
            tip,
            left,
            rightPoint,
            IM_COL32(
                0,
                0,
                0,
                220
            ),
            2.0f
        );
    }
}

void MiniMapRadar::Draw(
    Scene& scene,
    int screenWidth,
    int screenHeight
)
{
    if (!enabled)
        return;

    SceneObject* player =
        FindPlayerObjectLocal(
            scene
        );

    if (player == nullptr)
        return;

    ImDrawList* drawList =
        ImGui::GetForegroundDrawList();

    ImVec2 center =
        ImVec2(
            static_cast<float>(screenWidth) - radius - 28.0f,
            radius + 34.0f
        );

    float mapRadius =
        radius;

    drawList->AddCircleFilled(
        center,
        mapRadius + 8.0f,
        IM_COL32(
            0,
            0,
            0,
            145
        ),
        64
    );

    drawList->AddCircleFilled(
        center,
        mapRadius,
        IM_COL32(
            18,
            22,
            28,
            210
        ),
        64
    );

    drawList->AddCircle(
        center,
        mapRadius,
        IM_COL32(
            255,
            255,
            255,
            210
        ),
        64,
        2.0f
    );

    drawList->AddCircle(
        center,
        mapRadius * 0.66f,
        IM_COL32(
            255,
            255,
            255,
            55
        ),
        48,
        1.0f
    );

    drawList->AddCircle(
        center,
        mapRadius * 0.33f,
        IM_COL32(
            255,
            255,
            255,
            45
        ),
        48,
        1.0f
    );

    drawList->AddLine(
        ImVec2(center.x - mapRadius, center.y),
        ImVec2(center.x + mapRadius, center.y),
        IM_COL32(
            255,
            255,
            255,
            35
        ),
        1.0f
    );

    drawList->AddLine(
        ImVec2(center.x, center.y - mapRadius),
        ImVec2(center.x, center.y + mapRadius),
        IM_COL32(
            255,
            255,
            255,
            35
        ),
        1.0f
    );

    int coinCount =
        0;

    bool hasMonster =
        false;

    bool hasGate =
        false;

    bool hasNpc =
        false;

    bool hasTrigger =
        false;

    for (SceneObject* object : scene.objects)
    {
        if (object == nullptr)
            continue;

        if (!object->visible)
            continue;

        if (object == player)
            continue;

        MiniMapIconShape shape =
            MiniMapIconShape::Circle;

        ImU32 color =
            IM_COL32(
                255,
                255,
                255,
                255
            );

        bool shouldDraw =
            false;

        float iconSize =
            5.0f;

        if (IsCoinObject(object))
        {
            shouldDraw =
                true;

            coinCount++;

            shape =
                MiniMapIconShape::Circle;

            color =
                IM_COL32(
                    255,
                    215,
                    0,
                    255
                );

            iconSize =
                4.5f;
        }
        else if (IsTriggerZoneObject(object))
        {
            shouldDraw =
                true;

            hasTrigger =
                true;

            shape =
                MiniMapIconShape::Diamond;

            color =
                IM_COL32(
                    255,
                    90,
                    90,
                    255
                );

            iconSize =
                6.0f;
        }
        else if (IsMonsterSpawnObject(object))
        {
            shouldDraw =
                true;

            hasMonster =
                true;

            shape =
                MiniMapIconShape::Triangle;

            color =
                IM_COL32(
                    190,
                    70,
                    255,
                    255
                );

            iconSize =
                7.0f;
        }
        else if (IsMusicGateObject(object))
        {
            shouldDraw =
                true;

            hasGate =
                true;

            shape =
                MiniMapIconShape::Square;

            color =
                IM_COL32(
                    80,
                    180,
                    255,
                    255
                );

            iconSize =
                6.0f;
        }
        else if (IsMusicNpcObject(object))
        {
            shouldDraw =
                true;

            hasNpc =
                true;

            shape =
                MiniMapIconShape::Ring;

            color =
                IM_COL32(
                    120,
                    255,
                    160,
                    255
                );

            iconSize =
                6.5f;
        }

        if (!shouldDraw)
            continue;

        ImVec2 radarPosition =
            GetMiniMapScreenPosition(
                player->transform.position,
                object->transform.position,
                center,
                mapRadius - 12.0f,
                range
            );

        DrawMiniMapIcon(
            drawList,
            radarPosition,
            color,
            shape,
            iconSize
        );
    }

    DrawMiniMapPlayerArrow(
        drawList,
        center,
        player->transform.rotation.y
    );

    const char* title =
        "RADAR";

    ImVec2 titleSize =
        ImGui::CalcTextSize(
            title
        );

    drawList->AddText(
        ImVec2(
            center.x - titleSize.x * 0.5f,
            center.y - mapRadius - 28.0f
        ),
        IM_COL32(
            255,
            255,
            255,
            240
        ),
        title
    );

    std::string infoText =
        "Coins: " +
        std::to_string(
            coinCount
        );

    if (hasMonster)
        infoText += "  M";

    if (hasGate)
        infoText += "  G";

    if (hasNpc)
        infoText += "  NPC";

    if (hasTrigger)
        infoText += "  T";

    ImVec2 infoSize =
        ImGui::CalcTextSize(
            infoText.c_str()
        );

    drawList->AddRectFilled(
        ImVec2(
            center.x - infoSize.x * 0.5f - 8.0f,
            center.y + mapRadius + 10.0f
        ),
        ImVec2(
            center.x + infoSize.x * 0.5f + 8.0f,
            center.y + mapRadius + 30.0f
        ),
        IM_COL32(
            0,
            0,
            0,
            150
        ),
        5.0f
    );

    drawList->AddText(
        ImVec2(
            center.x - infoSize.x * 0.5f,
            center.y + mapRadius + 13.0f
        ),
        IM_COL32(
            255,
            255,
            255,
            230
        ),
        infoText.c_str()
    );
}