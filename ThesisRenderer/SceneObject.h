#pragma once

#include "Mesh.h"
#include "Shader.h"
#include <glm/glm.hpp>

class SceneObject
{
public:

    Mesh* mesh;
    Shader* shader;

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    SceneObject(Mesh* m, Shader* s);

    glm::mat4 GetModelMatrix();
};