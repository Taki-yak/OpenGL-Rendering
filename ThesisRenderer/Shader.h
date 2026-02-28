#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader
{
public:

    unsigned int ID;

    Shader(const char* vertexSource, const char* fragmentSource);

    void use();

    void setMat4(const std::string& name, const glm::mat4& matrix);
};