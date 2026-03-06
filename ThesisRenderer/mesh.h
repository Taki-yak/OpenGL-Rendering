#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

class Mesh
{
public:

    unsigned int VAO;
    unsigned int VBO;

    Mesh(float* vertices, int size);

    void Draw();
};

#endif