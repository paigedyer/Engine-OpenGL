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
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
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

        // Multiply by sensitivity, without it, the movement would be too fast and too strong
        const float sensitivity = 0.1f; // Change this value as needed
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // Constraining the pitch
        if (pitch > 89.0f)
        {
            pitch = 89.0f;
        }
        if (pitch < -89.0f)
        {
            pitch = -89.0f;
        }

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        glm::vec3 camAngle = glm::normalize(front);
        engine->SetCameraAngle(camAngle);
    }
    else
    {
        std::cout << "ENGINE REF BAD IN MOUSE CALLBACK";
    }
}



void Engine::StartRenderLoop()
// Called after SetupGLFW()
{
    // Compile shaders
    Shader shader("../src/Shaders/shader.vs", "../src/Shaders/shader.fs");

    CreateVertexArray();

    // Texture
    ApplyTexture();
    shader.Use();
    shader.setInt("texture", 0);

    // Sets camera variable values
    SetupCamera();

    // Matrices for translations
    CreateMatrices(shader);

    // Enable depth
    glEnable(GL_DEPTH_TEST);
    projection = glm::perspective(glm::radians(45.0f), (float)winX / (float)winY, 0.1f, 100.0f);
    shader.setMat4("projection", projection);

    while (!glfwWindowShouldClose(window))
    {
        CalculateDeltaTime();
        // Keep running

        // Input
        ProcessInput(window);

        glClearColor(0.8f, 0.973f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);

        // Rendering Commands

        // Activate the shader
        shader.Use();

        view = glm::mat4(1.0f);

        // Rotate cube over time
        // model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));

        float radius = 10.0f;
        float camX = static_cast<float>(sin(glfwGetTime()) * radius);
        float camZ = static_cast<float>(cos(glfwGetTime()) * radius);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        projection = glm::perspective(glm::radians(45.0f), (float)winX / (float)winY, 0.1f, 100.0f);

        shader.setMat4("view", view);

        model = glm::mat4(1.0f);
        shader.setMat4("model", model);

        // Render
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);

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

// TIME
void Engine::CalculateDeltaTime()
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
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
    const float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
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
    // SHAPE TO SET UP
    //SetupTriangle();
    //SetupRectangle();
    SetupCube();

    // Set vertex attributes pointers

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color attribute
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);

    // Texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void Engine::SetupCamera()
{
    // Position
    cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);

    // Direction
    cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    cameraDirection = glm::normalize(cameraPos - cameraTarget);

    // Right axis
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    cameraRight = glm::normalize(glm::cross(up, cameraDirection));

    // Up axis
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

}

void Engine::SetCameraAngle(glm::vec3 cameraAngle)
{
    cameraFront = glm::normalize(cameraAngle);
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

void Engine::SetupCube()
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
}

void Engine::ApplyTexture()
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    // Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    // Flip the texture on the y-axis
    stbi_set_flip_vertically_on_load(true);


    unsigned char *data = stbi_load("../textures/texture.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        // Generate Mipmaps
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

void Engine::CreateMatrices(Shader s)
{
    projection = glm::mat4(1.0f);
    s.setMat4("projection", projection);
}



