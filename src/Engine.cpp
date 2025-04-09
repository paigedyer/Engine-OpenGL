#define GLFW_INCLUDE_NONE
#include <iostream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Engine.h>

Engine::Engine()
{

    // Window size
    winX = 640;
    winY = 480;
    // Title of Window set to Default.
    winTitle = "Default";

}

void Engine::SetupGLFW()
// Initializes glfw and GLAD; Opens a window, registers callbacks
{
	if(!glfwInit()) 
	{
		// Failed initialization
	}

	// Selecting OpenGL profile. Using Core profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a window and context
	CreateWindow();

	// Makes a current OpenGL context
	glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        // return -1;
    }

    // Tell OpenGL the size of the window
    glViewport(0, 0, winX, winY);

    // Register a callback function
    // Takes a GLFWwindow as its first argument and two integers indicating the new window's dimensions. Whenever
    // the window changes in size, GLFW calles this function and fills in the proper arguments.
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
	
}

void Engine::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Viewport adjusts to new size
    glViewport(0, 0, width, height);
} 

void Engine::StartRenderLoop()
// Called after SetupGLFW()
{
    while (!glfwWindowShouldClose(window))
    {
        // Keep running

        // Input

        glfwPollEvents();
        // Checks if any events are triggered like:
        //  - Keyboard input
        //  - Mouse Movement
        // Updates the window state
        // Calls corresponding functions

        // Rendering Commands

        glfwSwapBuffers(window);
        // What is a color buffer??
        // -> Large 2D buffer that contains color values for each pixel in GLFW's window
        // What is a 'Double Buffer'?
        // -> When an app draws a single buffer, the image might flicker. This is because the output image isn't 
        //    drawn in an instant, it's pixel-by-pixel
        // + Front Buffer: Final output image on the screen
        // + Back Buffer: Where rendering commands are drawn to.
        //      When rendering commands are finished -> Swap back to the front.


        
    }
    DestroyWindow();

}

void Engine::CreateWindow()
// Create a window and context
{
    window = glfwCreateWindow(winX, winY, winTitle, NULL, NULL);
    if (!window)
    {
        // Window or OpenGL context creation failed
        // Check the return value
        // Proper drivers

        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();

        // return -1;
    }
}

void Engine::DestroyWindow()
{
    if (window)
    {
        glfwDestroyWindow(window);
        // return 0;
    }
}

