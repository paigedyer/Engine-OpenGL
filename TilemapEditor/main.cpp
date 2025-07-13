#define GLFW_INCLUDE_NONE
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Engine.h>

int main()
{
    Engine newEngineInstance;
    newEngineInstance.SetupGLFW();
    newEngineInstance.StartRenderLoop();
    return 0;
}