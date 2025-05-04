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

    void SetBufferColor(float r, float g, float b, float a);
    void InitColors();
    void ChangeColor();

    // TRIANGLE
    void SetupTriangle();
    unsigned int VBO;

    // SHADERS
    void SetupShaders();
    void SetupVertexShader();
    void SetupFragmentShader();
    void CreateShaderProgram();

    // VAO
    void CreateVertexArray();

    // Input
    void ProcessInput(GLFWwindow *window);

    // Callback functions
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    // Struct
    struct Color {
        float r;
        float g;
        float b;
        float a;
    
        // Default constructor (initializes to black)
        Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}
        
        Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
    };

    // VARIABLES

    // Window
    GLFWwindow* window;

    // Window size
    int winX, winY;
    const char* winTitle;

    // SHADERS
    unsigned int vertexShader;
    const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

    unsigned int fragmentShader;
    const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);}\0";

    unsigned int shaderProgram;

    // VAO
    unsigned int VAO;

    // Colors
    Color Red;
    Color Black;
    Color Blue;
    Color Pink;
    Color White;

    Color colorsArr[5];
    int index;

    // TRIANGLE

    float triVertices[9] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };

};

#endif