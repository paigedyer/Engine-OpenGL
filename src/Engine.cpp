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
    SetupShaders();
    SetupTriangle();
    // Create Vertex Array
    CreateVertexArray();

    while (!glfwWindowShouldClose(window))
    {
        // Keep running

        // Input
        ProcessInput(window);

        glClearColor(1.0f, 0.973f, 0.941f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Rendering Commands
        glUseProgram(shaderProgram); // Uses the shader for rendering calls
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

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

// VAO
void Engine::CreateVertexArray()
{
    glGenVertexArrays(1, &VAO);
    // Bind Vertex Array Object
    glBindVertexArray(VAO);
    // Copy vertices array in a buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triVertices), triVertices, GL_STATIC_DRAW);
    // Set vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

// TRIANGLE
void Engine::SetupTriangle()
{

    glGenBuffers(1, &VBO); // Generates a buffer ID

    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Any buffer calls on the GL_ARRAY_BUFFER type will be used to config the currently bound buffer
    // Copies vertext data into the buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(triVertices), triVertices, GL_STATIC_DRAW);
}



