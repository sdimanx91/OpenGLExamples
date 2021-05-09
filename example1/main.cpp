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
                           "uniform float xOffset;"
                           "out vec3 ourColor;"
                           "void main() {"
                           "    gl_Position = vPos;"
                           "    gl_Position.w = 1.f;"
                           "    gl_Position.x += xOffset;"
                           "    ourColor = vec3(gl_Position.x, gl_Position.y, gl_Position.x * gl_Position.y);"
                           "}";

const char* fragmentShader = "#version 330 core \n"
                             "in vec3 ourColor;"
                             "out vec3 color;"
                             "void main() {"
                             "    color = ourColor;"
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

GLuint createTriangle()
{
    GLuint vertexArrayName;

    glGenVertexArrays(1 /*array elemets count*/, &vertexArrayName /*array*/);
    glBindVertexArray(vertexArrayName);

    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f,
    };

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    return vertexBuffer;
}

void draw(GLuint vertexBufferName, GLuint program, GLfloat offset)
{
    GLint offsetLocation = glGetUniformLocation(program, "xOffset");
    glUniform1f(offsetLocation, offset);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferName);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);
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

    GLfloat offset = -1;
    GLfloat mult = 1;
    do
    {
        glClearColor(1.f, 1.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto triangleBufferId = createTriangle();

        glUseProgram(programId);

        draw(triangleBufferId, programId, offset);

        glfwSwapBuffers(window);
        glfwPollEvents();

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(20ms); //~48 frames / sec
        offset += mult * 0.01;

        if (offset > 1.f) {
            offset = 1.f;
            mult = -1.f;
        }
        else if (offset < -1.f)
        {
            offset = -1.f;
            mult = 1.f;
        }

    } // Проверяем нажатие клавиши Escape или закрытие окна
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    glDeleteProgram(programId);
}
