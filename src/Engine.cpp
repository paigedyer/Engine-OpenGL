#define GLFW_INCLUDE_NONE
#include <complex>
#include <iostream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

#include <SHADER.h>

#include <Engine.h>

Engine::Engine()
{
    // Window size
    winX = 640;
    winY = 480;
    // Title of Window set to Default.
    winTitle = "Default";

    index = 0;

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

    InitColors();
	
}

void Engine::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Viewport adjusts to new size
    glViewport(0, 0, width, height);
}

void Engine::StartRenderLoop()
// Called after SetupGLFW()
{
    // Compile shaders
    Shader shader("../src/Shaders/shader.vs", "../src/Shaders/shader.fs");

    //SetupTriangle();

    CreateVertexArray();
    SetupRectangle();
    CreateMatrices();
    ApplyTexture();

    while (!glfwWindowShouldClose(window))
    {
        // Keep running

        // Input
        ProcessInput(window);

        glClearColor(0.8f, 0.973f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);

        // Rendering Commands
        int vertexColorLocation = glGetUniformLocation(shader.ID, "color");
        shader.Use();

        // "4f" because it expects 4 float values
        glUniform4f(vertexColorLocation, 1.0f, 0.0f, 0.0f, 1.0f); // Color to fragment shader
        int modelLoc = glGetUniformLocation(shader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        int viewLoc = glGetUniformLocation(shader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projectionLoc = glGetUniformLocation(shader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
    DestroyWindow();
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    //glDeleteProgram(shaderProgram);
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

// INPUT

void Engine::ProcessInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        std::cout << "ESCAPE" << std::endl;
        glfwSetWindowShouldClose(window, true);
    }
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        std::cout << "SPACE" << std::endl;
        ChangeColor();
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

/* MOVED TO SHADER.H
// SHADERS
void Engine::SetupShaders()
{
    SetupVertexShader();
    SetupFragmentShader();
    CreateShaderProgram();
}

void Engine::SetupVertexShader()
{
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Attach source code to the shader object
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader); // Compile shader

    // Check for compilation errors
    int success;
    char infoLog[512]; // Storage for error messages
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); // Checks for success
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

void Engine::SetupFragmentShader()
{
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check for compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::FRAGMENT_SHADER\n" << infoLog << std::endl;
    }
}

void Engine::CreateShaderProgram()
{
    // Creates a program and returns the ID reference
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // Links the shaders by the corresponding/matching input and outputs
    glLinkProgram(shaderProgram);

    // Check for compilation errors
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);\
        std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    else {
        // Delete the vertex and fragment shaders once they've been linked
        std::cout << "Program linked successfully" << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
}
*/

// VAO
void Engine::CreateVertexArray()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind Vertex Array Object
    glBindVertexArray(VAO);
    // Copy vertices array in a buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);
    // Set vertex attributes pointers

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}


// TRIANGLE
void Engine::SetupTriangle()
{

    glGenBuffers(1, &VBO); // Generates a buffer ID

    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Any buffer calls on the GL_ARRAY_BUFFER type will be used to config the currently bound buffer
    // Copies vertext data into the buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(triVertices), triVertices, GL_STATIC_DRAW);
}

void Engine::SetupRectangle()
{

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void Engine::ApplyTexture()
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("../textures/texture.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }


    // Free the image memory
    stbi_image_free(data);
}


void Engine::CreateMatrices()
{
    model = glm::mat4(1.0f);
    // Rotates the object so that it's flat
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
}



