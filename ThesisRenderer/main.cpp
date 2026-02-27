#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


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

    if (pitch > 89.0f)
        pitch = 89.0f;

    if (pitch < -89.0f)
        pitch = -89.0f;

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
layout (location = 1) in vec3 aColor;

out vec3 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertexColor = aColor;
}
)";


const char* fragmentShaderSource = R"(
#version 330 core

in vec3 vertexColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(vertexColor, 1.0);
}
)";



// ================= MAIN =================

int main()
{

    // Initialize GLFW
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 600, "ThesisRenderer", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);


    // Mouse settings
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }


    glEnable(GL_DEPTH_TEST);



    // ================= CUBE DATA =================

    float vertices[] =
    {
        // positions          // colors

        -0.5f,-0.5f,-0.5f,  1,0,0,
         0.5f,-0.5f,-0.5f,  0,1,0,
         0.5f, 0.5f,-0.5f,  0,0,1,
         0.5f, 0.5f,-0.5f,  0,0,1,
        -0.5f, 0.5f,-0.5f,  1,1,0,
        -0.5f,-0.5f,-0.5f,  1,0,0,

        -0.5f,-0.5f,0.5f,   1,0,1,
         0.5f,-0.5f,0.5f,   0,1,1,
         0.5f,0.5f,0.5f,    1,1,1,
         0.5f,0.5f,0.5f,    1,1,1,
        -0.5f,0.5f,0.5f,    0.5,0.5,0.5,
        -0.5f,-0.5f,0.5f,   1,0,1,

        -0.5f,0.5f,0.5f,    1,0,0,
        -0.5f,0.5f,-0.5f,   0,1,0,
        -0.5f,-0.5f,-0.5f,  0,0,1,
        -0.5f,-0.5f,-0.5f,  0,0,1,
        -0.5f,-0.5f,0.5f,   1,1,0,
        -0.5f,0.5f,0.5f,    1,0,0,

         0.5f,0.5f,0.5f,    1,0,1,
         0.5f,0.5f,-0.5f,   0,1,1,
         0.5f,-0.5f,-0.5f,  1,1,1,
         0.5f,-0.5f,-0.5f,  1,1,1,
         0.5f,-0.5f,0.5f,   0.5,0.5,0.5,
         0.5f,0.5f,0.5f,    1,0,1,

        -0.5f,-0.5f,-0.5f,  1,0,0,
         0.5f,-0.5f,-0.5f,  0,1,0,
         0.5f,-0.5f,0.5f,   0,0,1,
         0.5f,-0.5f,0.5f,   0,0,1,
        -0.5f,-0.5f,0.5f,   1,1,0,
        -0.5f,-0.5f,-0.5f,  1,0,0,

        -0.5f,0.5f,-0.5f,   1,0,1,
         0.5f,0.5f,-0.5f,   0,1,1,
         0.5f,0.5f,0.5f,    1,1,1,
         0.5f,0.5f,0.5f,    1,1,1,
        -0.5f,0.5f,0.5f,    0.5,0.5,0.5,
        -0.5f,0.5f,-0.5f,   1,0,1
    };



    // ================= VAO VBO =================

    unsigned int VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);



    // ================= SHADER =================

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);


    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);


    unsigned int shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);


    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);



    // ================= LOOP =================

    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = glfwGetTime();

        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        float cameraSpeed = 2.5f * deltaTime;


        glfwPollEvents();


        // Keyboard movement

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraFront;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraFront;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;



        glClearColor(0.1f, 0.1f, 0.1f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        glm::mat4 model = glm::mat4(1);

        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));


        glm::mat4 view =
            glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);


        glm::mat4 projection =
            glm::perspective(glm::radians(45.0f),
                800.0f / 600.0f,
                0.1f,
                100.0f);



        glUseProgram(shaderProgram);


        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"),
            1, GL_FALSE, glm::value_ptr(model));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"),
            1, GL_FALSE, glm::value_ptr(view));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"),
            1, GL_FALSE, glm::value_ptr(projection));


        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, 36);


        glfwSwapBuffers(window);

    }



    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();

    return 0;
}