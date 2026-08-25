#pragma once

#include <glm/glm.hpp>

#define MAX_BONE_INFLUENCE 4

struct AnimatedVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;

    int boneIDs[MAX_BONE_INFLUENCE];

    float weights[MAX_BONE_INFLUENCE];

    AnimatedVertex()
    {
        position =
            glm::vec3(
                0.0f
            );

        normal =
            glm::vec3(
                0.0f,
                1.0f,
                0.0f
            );

        texCoords =
            glm::vec2(
                0.0f
            );

        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            boneIDs[i] =
                -1;

            weights[i] =
                0.0f;
        }
    }
};