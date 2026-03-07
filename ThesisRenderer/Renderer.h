#pragma once

#include "Shader.h"
#include "Mesh.h"
#include <glm/glm.hpp>

class Renderer
{
public:

    void DrawMesh(Mesh& mesh, Shader& shader, glm::mat4 model);
};