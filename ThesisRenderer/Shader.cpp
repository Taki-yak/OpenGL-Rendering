#include "Shader.h"
#include <iostream>

Shader::Shader(const char* vertexSource, const char* fragmentSource)
{
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);

    ID = glCreateProgram();

    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);

    glLinkProgram(ID);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::setMat4(const std::string& name, const glm::mat4& matrix)
{
    glUniformMatrix4fv(
        glGetUniformLocation(ID, name.c_str()),
        1,
        GL_FALSE,
        &matrix[0][0]
    );
}