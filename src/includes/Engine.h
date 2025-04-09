#ifndef ENGINE_H
#define ENGINE_H

#define GLFW_INCLUDE_NONE

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

class Engine {

public:

    Engine();

    void SetupGLFW();
    void CreateWindow();
    void DestroyWindow();
    void StartRenderLoop();

    // Callback functions
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    // Variables
    // Window
    GLFWwindow* window;

    // Window size
    int winX, winY;
    const char* winTitle;


};

#endif