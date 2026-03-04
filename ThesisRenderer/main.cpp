#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

// ================= CAMERA VARIABLES =================

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = 0.0f;

float lastX = 400;
float lastY = 300;

bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;


// ================= MOUSE CALLBACK =================

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float sensitivity = 0.1f;

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;

    lastX = (float)xpos;
    lastY = (float)ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(direction);
}


// ================= SHADERS =================
const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)"; const char* fragmentShaderSource = R"(
#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    // Ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0),32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
   FragColor = vec4(result, 1.0);
}
)";
const char* lightVertexSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* lightFragmentSource = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0); 
}
)";

// ================= MAIN =================

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "ThesisRenderer", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    // ================= SHADER CLASS =================

    Shader shader(vertexShaderSource, fragmentShaderSource);

    // ================= CUBE DATA =================

    float vertices[] = {
        // positions          // normals           // texcoords

        // Front face
        -0.5f,-0.5f, 0.5f,    0.0f,0.0f,1.0f,     0.0f,0.0f,
         0.5f,-0.5f, 0.5f,    0.0f,0.0f,1.0f,     1.0f,0.0f,
         0.5f, 0.5f, 0.5f,    0.0f,0.0f,1.0f,     1.0f,1.0f,
         0.5f, 0.5f, 0.5f,    0.0f,0.0f,1.0f,     1.0f,1.0f,
        -0.5f, 0.5f, 0.5f,    0.0f,0.0f,1.0f,     0.0f,1.0f,
        -0.5f,-0.5f, 0.5f,    0.0f,0.0f,1.0f,     0.0f,0.0f,

        // Back face
        -0.5f,-0.5f,-0.5f,    0.0f,0.0f,-1.0f,    1.0f,0.0f,
        -0.5f, 0.5f,-0.5f,    0.0f,0.0f,-1.0f,    1.0f,1.0f,
         0.5f, 0.5f,-0.5f,    0.0f,0.0f,-1.0f,    0.0f,1.0f,
         0.5f, 0.5f,-0.5f,    0.0f,0.0f,-1.0f,    0.0f,1.0f,
         0.5f,-0.5f,-0.5f,    0.0f,0.0f,-1.0f,    0.0f,0.0f,
        -0.5f,-0.5f,-0.5f,    0.0f,0.0f,-1.0f,    1.0f,0.0f,

        // Left face
        -0.5f, 0.5f, 0.5f,   -1.0f,0.0f,0.0f,     1.0f,0.0f,
        -0.5f, 0.5f,-0.5f,   -1.0f,0.0f,0.0f,     1.0f,1.0f,
        -0.5f,-0.5f,-0.5f,   -1.0f,0.0f,0.0f,     0.0f,1.0f,
        -0.5f,-0.5f,-0.5f,   -1.0f,0.0f,0.0f,     0.0f,1.0f,
        -0.5f,-0.5f, 0.5f,   -1.0f,0.0f,0.0f,     0.0f,0.0f,
        -0.5f, 0.5f, 0.5f,   -1.0f,0.0f,0.0f,     1.0f,0.0f,

        // Right face
         0.5f, 0.5f, 0.5f,    1.0f,0.0f,0.0f,     1.0f,0.0f,
         0.5f,-0.5f,-0.5f,    1.0f,0.0f,0.0f,     0.0f,1.0f,
         0.5f, 0.5f,-0.5f,    1.0f,0.0f,0.0f,     1.0f,1.0f,
         0.5f,-0.5f,-0.5f,    1.0f,0.0f,0.0f,     0.0f,1.0f,
         0.5f, 0.5f, 0.5f,    1.0f,0.0f,0.0f,     1.0f,0.0f,
         0.5f,-0.5f, 0.5f,    1.0f,0.0f,0.0f,     0.0f,0.0f,

         // Bottom face
         -0.5f,-0.5f,-0.5f,    0.0f,-1.0f,0.0f,    0.0f,1.0f,
          0.5f,-0.5f,-0.5f,    0.0f,-1.0f,0.0f,    1.0f,1.0f,
          0.5f,-0.5f, 0.5f,    0.0f,-1.0f,0.0f,    1.0f,0.0f,
          0.5f,-0.5f, 0.5f,    0.0f,-1.0f,0.0f,    1.0f,0.0f,
         -0.5f,-0.5f, 0.5f,    0.0f,-1.0f,0.0f,    0.0f,0.0f,
         -0.5f,-0.5f,-0.5f,    0.0f,-1.0f,0.0f,    0.0f,1.0f,

         // Top face
         -0.5f, 0.5f,-0.5f,    0.0f,1.0f,0.0f,     0.0f,1.0f,
         -0.5f, 0.5f, 0.5f,    0.0f,1.0f,0.0f,     0.0f,0.0f,
          0.5f, 0.5f, 0.5f,    0.0f,1.0f,0.0f,     1.0f,0.0f,
          0.5f, 0.5f, 0.5f,    0.0f,1.0f,0.0f,     1.0f,0.0f,
          0.5f, 0.5f,-0.5f,    0.0f,1.0f,0.0f,     1.0f,1.0f,
         -0.5f, 0.5f,-0.5f,    0.0f,1.0f,0.0f,     0.0f,1.0f
    };
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // ================= TEXTURE =================

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("D:\\taki\\POLAND\\POLAND\\ThesisRenderer\\ThesisRenderer\\container.jpg", &width, &height, &nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
            width, height, 0,
            GL_RGB, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
        std::cout << "Failed to load texture\n";

    stbi_image_free(data);

    shader.use();
    Shader lightShader(lightVertexSource, lightFragmentSource);
    glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);
    shader.setVec3("lightPos", glm::vec3(1.2f, 1.0f, 2.0f));
    shader.setVec3("viewPos", cameraPos);
    shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
  
    // ================= RENDER LOOP =================

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        float cameraSpeed = 2.5f * deltaTime;

        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::rotate(glm::mat4(1.0f),
            (float)glfwGetTime(),
            glm::vec3(0.5f, 1.0f, 0.0f));

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            800.0f / 600.0f,
            0.1f,
            100.0f);
        // ===== DRAW MAIN CUBE =====
        shader.use();

        shader.setMat4("model", glm::value_ptr(model));
        shader.setMat4("view", glm::value_ptr(view));
        shader.setMat4("projection", glm::value_ptr(projection));

        shader.setVec3("lightPos", glm::vec3(2.0f, 2.0f, 2.0f));
        shader.setVec3("viewPos", cameraPos);
        shader.setVec3("lightColor", glm::vec3(1.0f));
        shader.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.3f));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // ===== DRAW LIGHT CUBE =====
        lightShader.use();

        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, glm::vec3(2.0f, 2.0f, 2.0f));
        lightModel = glm::scale(lightModel, glm::vec3(0.2f));

        lightShader.setMat4("model", glm::value_ptr(lightModel));
        lightShader.setMat4("view", glm::value_ptr(view));
        lightShader.setMat4("projection", glm::value_ptr(projection));

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();

    return 0;
}