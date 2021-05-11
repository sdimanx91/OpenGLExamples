#include <iostream>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <thread>
#include <chrono>

using namespace glm;


GLuint compileShader(const std::string& source, GLenum shaderType)
{
    if (source.empty())
        return false;

    GLuint shaderID = glCreateShader(shaderType);

    // compile
    {
        const char * sourcePtr = source.c_str();
        glShaderSource(shaderID, 1, &sourcePtr, NULL);
        glCompileShader(shaderID);
    }

    // validate
    {
        GLint result = GL_FALSE;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);

        if (result == GL_TRUE)
            std::cout << "Shader success initialized: " << shaderID << std::endl;
        else
            std::cout << "Shader error: " << shaderID << std::endl;

        GLint logLength = 0;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);

        if (logLength > 0)
        {
            std::vector<char> log(logLength + 1);
            glGetShaderInfoLog(shaderID, logLength, nullptr, &log[0]);

            std::cout << "ShaderLog: " << &log[0] << std::endl;
        }

    }

    return shaderID;
}

const char* vertexShader = "#version 330 core \n"
                           "layout(location = 0) in vec4 vPos;"
                           "void main() {"
                           "    gl_Position = vPos;"
                           "    gl_Position.w = 1.f;"
                           "}";

const char* fragmentShader = "#version 330 core \n"
                             "out vec3 color;"
                             "void main() {"
                             "    color = vec3(1.f, 1.f, 0.f);"
                             "}";

GLuint createGLProgram()
{
    const GLuint programId = glCreateProgram();

    GLuint vertexShaderId;
    // vertexShader
    {
        vertexShaderId = compileShader(vertexShader, GL_VERTEX_SHADER);
        glAttachShader(programId, vertexShaderId);
    }

    GLuint fragmentShaderId;
    // fragmentShader
    {
        fragmentShaderId = compileShader(fragmentShader, GL_FRAGMENT_SHADER);
        glAttachShader(programId, fragmentShaderId);
    }

    glLinkProgram(programId);

    // printLog
    {
        GLint result = GL_FALSE;

        // linkStatus
        glGetProgramiv(programId, GL_LINK_STATUS, &result);
        if (result == GL_TRUE)
            std::cout << "Program success initialized: " << programId << std::endl;
        else
            std::cout << "Program error: " << programId << std::endl;

        GLint logLength = 0;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);

        if (logLength > 0)
        {
            std::vector<char> log(logLength + 1);
            glGetProgramInfoLog(programId, logLength, nullptr, &log[0]);

            std::cout << "Programm log: " << &log[0] << std::endl;
        }
    }

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    return programId;
}

int main()
{
    // Инициализируем GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Ошибка при инициализации GLFWn");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);               // 4x Сглаживание
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Мы хотим использовать OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
                   GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Мы не хотим старый OpenGL

    // Открыть окно и создать в нем контекст OpenGL
    GLFWwindow* window; // (В сопроводительном исходном коде эта переменная является глобальной)
    window = glfwCreateWindow(1024, 768, "Tutorial 01", NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Инициализируем GLEW
    glewExperimental = true; // Флаг необходим в Core-режиме OpenGL
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Невозможно инициализировать GLEWn");
        return -1;
    }

    // Включим режим отслеживания нажатия клавиш, для проверки ниже
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // find vertexOffset and move vertexes
    auto programId = createGLProgram();

    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat vertices[] = {
        0.5f,  0.5f, 0.0f,  // Top Right
        0.5f, -0.5f, 0.0f,  // Bottom Right
        -0.5f, -0.5f, 0.0f,  // Bottom Left
        -0.5f,  0.5f, 0.0f   // Top Left
    };

    GLuint indexes[] = {
        0, 1, 3,
        1, 2, 3
    };

    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glUseProgram(programId);

    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw our first triangle
        glUseProgram(programId);
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}
