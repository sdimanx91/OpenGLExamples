#include <iostream>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "src/field.h"


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
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
                   GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Мы не хотим старый OpenGL

    // Открыть окно и создать в нем контекст OpenGL
    GLFWwindow* window; // (В сопроводительном исходном коде эта переменная является глобальной)
    window = glfwCreateWindow(1024, 768, "Tutorial 02", NULL, NULL);
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

    Field field;
    Field::Item currentMove = Field::Item::Circle;
    bool doPressed = false;

    do
    {
        glClearColor(1.f, 1.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        field.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();

        int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (state == GLFW_PRESS)
        {
            doPressed = true;
        }
        else if (doPressed)
        {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);


            int xi, yi;
            if (xpos < (1024/3)) {
                xi = 0;
            } else if (xpos < (2 * (1024/3))) {
                xi = 1;
            } else {
                xi = 2;
            }
            if (ypos < (768/3)) {
                yi = 0;
            } else if (ypos < (2 * (768/3))) {
                yi = 1;
            } else {
                yi = 2;
            }

            std::cout << "x = " << xpos <<" "<< xi<<" "<< ypos<<" "<< yi<<std::endl;


            field.set(currentMove, xi, yi);
            if (currentMove == Field::Circle)
                currentMove = Field::Cross;
            else
                currentMove = Field::Circle;

            doPressed = false;
        }

    } // Проверяем нажатие клавиши Escape или закрытие окна
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
}
