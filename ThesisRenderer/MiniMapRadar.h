#pragma once

#include "Scene.h"

class MiniMapRadar
{
public:
    bool enabled =
        true;

    float range =
        85.0f;

    float radius =
        96.0f;

    void Draw(
        Scene& scene,
        int screenWidth,
        int screenHeight
    );
};