#define GLFW_INCLUDE_NONE
#include <complex>
#include <iostream>
#include <string>
#include <filesystem>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "Model.h"
#include "Mesh.h"

#include <SHADER.h>

#include <Engine.h>

static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 0.0f;
float lastY = 0.0f;
float fov = 45.0f;

Engine::Engine()
{
    // Window size
    winX = 640;
    winY = 480;
    // Title of Window set to Default.
    winTitle = "Default";

    index = 0;

    deltaTime = 0.0f;
    lastFrame = 0.0f;
    yaw = -90.0f;
    pitch = 0.0f;
    lastX = winX / 2.0f;
    lastY = winY / 2.0f;
    fov = 45.0f;

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

    // REGISTER CALLBACK FUNCTIONS
    // Register a callback function
    // Takes a GLFWwindow as its first argument and two integers indicating the new window's dimensions. Whenever
    // the window changes in size, GLFW calles this function and fills in the proper arguments.
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Tell GLFW to caputure mouse
        // Capture = Once the app has focus, the mouse cursor stays within the center of the window.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    InitColors();
}

void Engine::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Viewport adjusts to new size
    glViewport(0, 0, width, height);
}

static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    cout << "MOUSE MOVED" << endl;
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    // TODO: Figure out a better way to do this :/
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));

    if (engine)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        engine->SetCameraAngle(xoffset, yoffset);
    }
    else
    {
        std::cout << "ENGINE REF BAD IN MOUSE CALLBACK";
    }
}

void Engine::StartRenderLoop()
// Called after SetupGLFW()
{
    // Flip loaded textures on y-axis
    stbi_set_flip_vertically_on_load(true);

    // Compile shaders
    Shader shader("../src/Shaders/shader.vs", "../src/Shaders/shader.fs");

    // Sets camera variable values
    SetupCamera();

    // Matrices for translations
    CreateMatrices(shader);

    // Set the path to the model
    const string sPath = "../Models/backpack/backpack.obj";
    // Load the model
    Model aModel(sPath);

    // Enable depth
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        CalculateDeltaTime();
        // Keep running

        // Input
        ProcessInput(window);

        // Color buffer
        glClearColor(0.8f, 0.973f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --------- RENDERING COMMANDS ---------

        // enable shader
        shader.Use();
        if (camera)
        {
            projection = glm::perspective(glm::radians(camera->Zoom), (float)winX / (float)winY, 0.1f, 100.0f);
            view = camera->GetViewMatrix();
        }
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // Render the loaded model
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // put at the center
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); // Scaling the model
        shader.setMat4("model", model);
        aModel.Draw(shader);

        /* -- Unused but here for reference --
        // Render
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Rotate cube over time
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
        */

        glfwPollEvents();
        // Checks if any events are triggered like:
        //  - Keyboard input
        //  - Mouse Movement
        // Updates the window state
        // Calls corresponding functions

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

    // Cleanup when closing the window
    DestroyWindow();
    glfwTerminate();

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

//  --------- TIME  ---------
void Engine::CalculateDeltaTime()
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

//  --------- INPUT ---------

void Engine::ProcessInput(GLFWwindow *window)
{
    // CLOSE WINDOW
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        std::cout << "ESCAPE" << std::endl;
        glfwSetWindowShouldClose(window, true);
    }
    // SPACE BAR
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        std::cout << "SPACE" << std::endl;
        ChangeColor();
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        if (camera)
        {
            camera->ProcessKeyboard(FORWARD, deltaTime);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        if (camera)
        {
            camera->ProcessKeyboard(BACKWARD, deltaTime);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        if (camera)
        {
            camera->ProcessKeyboard(LEFT, deltaTime);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        if (camera)
        {
            camera->ProcessKeyboard(RIGHT, deltaTime);
        }
    }
        
}

// COLOR BUFFER
void Engine::SetBufferColor(float r, float g, float b, float a)
{
    glClearColor(r,g,b,a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Engine::InitColors()
{
    Color Red = Color(0.184f, 0.525f, 0.537f, 1.1f);
    Color Black = Color(0.118f, 0.118f, 0.141f, 1.1f);
    Color Blue = Color(0.573f, 0.078f, 0.047f, 1.1f);
    Color Pink = Color(1.0f, 0.973f, 0.941f, 1.1f);
    Color White = Color(1.0f, 0.702f, 0.816f, 1.1f);

    colorsArr[0] = Red;
    colorsArr[1] = Black;
    colorsArr[2] = Blue;
    colorsArr[3] = Pink;
    colorsArr[4] = White;

}

void Engine::ChangeColor()
{

    Color newColor = colorsArr[index];
    SetBufferColor(newColor.r, newColor.g, newColor.b, newColor.a);
    index =+ 1;
    if (index > 4)
    { 
        index = 0;
    }
}

void Engine::SetupCamera()
{
    // Instantiate the camera
    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

}

void Engine::SetCameraAngle(float xoff, float yoff)
{
    if (camera)
    {
       camera->ProcessMouseMovement(xoff, yoff);
    }
}

void Engine::CreateMatrices(Shader s)
{
    projection = glm::mat4(1.0f);
    s.setMat4("projection", projection);
}



