#include "Renderer.h"
#include <glm/gtc/type_ptr.hpp>

void Renderer::DrawMesh(Mesh& mesh, Shader& shader, glm::mat4 model)
{
    shader.use();

    shader.setMat4("model", glm::value_ptr(model));

    mesh.Draw();
}

void Renderer::DrawObject(SceneObject& obj)
{
    obj.shader->use();

    glm::mat4 model = obj.GetModelMatrix();

    obj.shader->setMat4("model", glm::value_ptr(model));

    obj.mesh->Draw();
}