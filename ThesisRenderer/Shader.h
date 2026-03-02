#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>

class Shader
{
public:
    unsigned int ID;

    Shader(const char* vertexSource, const char* fragmentSource);

    void use();

    void setMat4(const std::string& name, const float* value) const;
};

#endif