#pragma once

#include "Scene.h"
#include "imgui.h"
#include <glm/glm.hpp>

class ObjectiveMarkerSystem
{
public:
    bool enabled =
        true;

    void Draw(
        Scene& scene,
        const glm::vec3& cameraPosition,
        const glm::mat4& view,
        const glm::mat4& projection,
        int screenWidth,
        int screenHeight
    );

private:
    enum class MarkerShape
    {
        Circle,
        Diamond,
        Triangle,
        Square,
        Ring
    };

private:
    bool WorldToScreenPosition(
        const glm::vec3& worldPosition,
        const glm::mat4& view,
        const glm::mat4& projection,
        int screenWidth,
        int screenHeight,
        ImVec2& screenPosition
    ) const;

    void DrawSingleMarker(
        const char* label,
        const glm::vec3& worldPosition,
        const glm::vec3& cameraPosition,
        const glm::mat4& view,
        const glm::mat4& projection,
        int screenWidth,
        int screenHeight,
        ImU32 color,
        MarkerShape shape
    );

    void DrawMarkers(
        Scene& scene,
        const glm::vec3& cameraPosition,
        const glm::mat4& view,
        const glm::mat4& projection,
        int screenWidth,
        int screenHeight
    );

    void DrawHeader(
        Scene& scene,
        int screenWidth
    );
};