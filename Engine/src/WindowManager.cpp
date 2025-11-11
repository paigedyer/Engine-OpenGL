#define GLFW_INCLUDE_NONE
#include <complex>
#include <iostream>
#include <string>
#include <filesystem>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Camera.h>
#include <glm/glm.hpp>

#include <WindowManager.h>

#include "Engine.h"

namespace WindowManager
{
    bool m_bMouseWheelPressed = false;
    Window* currentWindow;
    bool firstMouse = true;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float lastX = 0.0f;
    float lastY = 0.0f;
    float fov = 45.0f;

    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        // Viewport adjusts to new size
        glViewport(0, 0, width, height);
        if (currentWindow)
        {
            currentWindow->SetFramebufferValues(width, height);
        }
    }

    void window_focus_callback(GLFWwindow* window, int focused)
    {
        if (focused)
        {
            // Window gained input focus
            glfwMakeContextCurrent(window);
            Window* temp = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
            currentWindow = temp;
            std::cout << currentWindow->GetTitle() << " focused. " << std::endl;
        }
        else
        {
            // window lost input focus
        }
    }

    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
    {
        if (m_bMouseWheelPressed)
        {
            //std::cout << xposIn << " " << yposIn << std::endl;
            //std::cout << "MOUSE MOVED" << std::endl;
            float xpos = static_cast<float>(xposIn);
            float ypos = static_cast<float>(yposIn);
            //std::cout << xpos << " " << ypos << std::endl;

            if (firstMouse)
            {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }

            float xoffset = xpos - lastX;
            float yoffset = ypos - lastY;
            //std::cout << xoffset << " " << yoffset << std::endl;
            lastX = xpos;
            lastY = ypos;
            if (currentWindow)
            {
                //std::cout << currentWindow->GetTitle() << " x: " << xoffset << std::endl;
                //std::cout << currentWindow->GetTitle() << " y: " << yoffset << std::endl;
                currentWindow->TransformScreen(xoffset, yoffset);
            }
            else
            {
                std::cout << "CURRENT WINDOW POINTER INVALID" << std::endl;
            }
        }
    }

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        if (currentWindow)
        {
            if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
            {
                //std::cout << "MIDDLE MOUSE PRESSED" << std::endl;
                m_bMouseWheelPressed = true;
            }
            if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
            {
                // std::cout << "MIDDLE MOUSE RELEASED" << std::endl;
                m_bMouseWheelPressed = false;
                firstMouse = true;
                currentWindow->TransformScreen(0.0f, 0.0f);
            }
        }
    }

    static void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        if (currentWindow)
        {
            float zoom = static_cast<float>(yoffset);
            currentWindow->ZoomScreen(zoom);
        }
    }

    void Window::SetFocused()
    {
        glfwFocusWindow(m_window);
    }

    void Window::SetFramebufferValues(int width, int height)
    {
        m_winWidth = width;
        m_winHeight = height;
    }

    void Window::TransformScreen(float x, float z)
        // z value is either 1 or -1
    {
        z = -z;
        float ndcX = 2.0f * x / m_winWidth;
        float ndcY = 2.0f * z / m_winHeight;
        float scale = m_zoom * 5.0f;

        glm::vec3 screenDelta(ndcX * scale * 50.0f, 0.0f, ndcY * scale * 50.0f);

        glm::vec3 gridDelta = glm::vec3(m_invIsoMatrix * glm::vec4(screenDelta, 0.0f));

        m_gridOffset -= gridDelta;

    }

    void Window::ZoomScreen(float zoom)
    {
        m_zoom += -0.02f * zoom;
        // std::cout << m_zoom << std::endl;
    }

    void CloseGLFW()
    {
        glfwTerminate();
    }

    Window::Window(unsigned int width, unsigned int height, const char* title, GLFWwindow* winContext)
        : m_winWidth(width)
          , m_winHeight(height)
          , m_winTitle(title)
          , m_winContext(winContext)
    {
        CreateWindow();
    }

    bool Window::CreateWindow()
    {
        // Selecting OpenGL profile. Using Core profile
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_window = glfwCreateWindow(m_winWidth, m_winHeight, m_winTitle, NULL, m_winContext);

        if (!m_window)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            return false;
        }

        glfwMakeContextCurrent(m_window);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return false;
        }

        glViewport(0, 0, m_winWidth, m_winHeight);
        glDisable(GL_DEPTH_TEST);

        // Grid Shader program
        m_shaderPtr = new Shader("../TilemapEditor/Shaders/shader.vs", "../TilemapEditor/Shaders/shader.fs");
        // UI shader program
        m_uiShaderPtr = new Shader("../TilemapEditor/Shaders/ui_shader.vs", "../TilemapEditor/Shaders/ui_shader.fs");
        m_uiShaderPtr->setVec3("chColor", glm::vec3(0.98f, 0.03f, 0.84));

        // Callback functions
        glfwSetCursorPosCallback(m_window, mouse_callback);
        glfwSetMouseButtonCallback(m_window, mouse_button_callback);
        glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
        glfwSetWindowFocusCallback(m_window, window_focus_callback);
        glfwSetWindowUserPointer(m_window, reinterpret_cast<void *>(this));
        glfwSetScrollCallback(m_window, mouse_scroll_callback);

        SetupCamera();

        m_isoMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        m_isoMatrix = glm::scale(m_isoMatrix, glm::vec3(0.5f, 1.0f, 1.0f));
        m_isoMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0, 1, 0));
        m_invIsoMatrix = glm::inverse(m_isoMatrix);

        return true;
    }

    bool Window::Init()
    {
        if (!glfwInit())
        {
            std::cout << "Failed to initialize GLFW" << std::endl;
            return false;
        }
        return true;
    }

    void Window::Update()
    {
        CalculateDeltaTime();
        glfwPollEvents();
        ProcessInput();
        Clear();

        m_shaderPtr->Use();

        // Reset m_model to identity matrix
        m_model = glm::mat4(1.0f);

        /////////////////// TRANSLATIONS ///////////////////////

        // "Squish in half" for isometric POV
        m_model = glm::scale(m_model, glm::vec3(0.5f, 1.0f, 1.0f));
        // Setting the proper angle for isometric POV
        m_model = glm::rotate(m_model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::vec4 worldOffset = m_model * glm::vec4(m_gridOffset, 1.0f);

        m_model = glm::translate(m_model, m_gridOffset);

            // Calculate tile value in integer
        glm::ivec2 tile = glm::floor(glm::vec2(m_gridOffset.x, m_gridOffset.z) / 5.0f);
            // Calculate exact tile value
        glm::vec2 exactTile = glm::vec2(m_gridOffset.x, m_gridOffset.z) / 5.0f;

        ////////////////////// CAMERA //////////////////////////
        if (m_camera)
        {
            m_projection = glm::ortho(
                (-(m_winWidth / 2.0f) * m_zoom), // LEFT
                ((m_winWidth / 2.0f) * m_zoom), // RIGHT
                ((m_winHeight / 2.0f) * m_zoom), // BOTTOM
                (-(m_winHeight / 2.0f) * m_zoom), // TOP
                (200.0f * 5), // ZNEAR
                (-200.0f * 5) // ZFAR
                );

            m_view = m_camera->GetViewMatrix();

        }
        else
        {
            std::cout << "Camera is NULL" << std::endl;
        }

        ///////////// SHADERS AND DRAWING /////////////////////////
        // Fills in data inside shaders for the grid
        SetShaderData(m_shaderPtr);

        // Draw calls
        glLineWidth(1.0f);
        DrawGridLines();

        // UI/HUD
        glLineWidth(2.0f);
        m_uiShaderPtr->Use();
        DrawUI();

        //////////////////// ON FRAME ////////////////////////////
        // Things to do every frame, not every tick
        if ((glfwGetTime() - m_lastFrameTime) >= m_fixedDeltaTime)
        {
            glfwSwapBuffers(m_window);

            m_lastFrameTime = glfwGetTime();

            /* PRINT GRID COORDS UNSCALED
            std::cout << "Grid coordinate: ("
          << m_gridOffset.x << ", "
          << m_gridOffset.z << ")" << std::endl;
            */

            /* Print Grid Coords Scaled and Floor
            std::cout << "Grid coordinate: ("
          << tile.x << ", "
          << tile.y << ")" << std::endl;
          */

            /* Print Grid Coords Scaled with decimals
            std::cout << "Grid coordinate: ("
          << exactTile.x << ", "
          << exactTile.y << ")" << std::endl;
          */
        }

    }

    void Window::onUpdate()
    // PUBLIC function for calling update functions
    {
        Update();
    }

    glm::vec3 Window::UnProject(glm::vec3 inVec3)
    {
        return glm::unProject(inVec3, m_view, m_projection, glm::vec4(0.0f, 0.0f, m_winWidth, m_winHeight));
    }

    glm::vec3 Window::Project(glm::vec3 inVec3)
    {
        return glm::project(inVec3, m_view, m_projection, glm::vec4(0.0f, 0.0f, m_winWidth, m_winHeight));
    }

    void Window::CalculateDeltaTime()
    {
        double currentFrame = glfwGetTime();
        m_deltaTime = currentFrame - m_lastUpdate;
        m_lastUpdate = currentFrame;

    }

    void Window::ProcessInput()
    {
        if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
        {
        }
        if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        {
        }
        if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        {

        }
        if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        {

        }
    }

    void Window::Clear()
    {
        glClearColor(0.8f, 0.973f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Window::SetupCamera()
    {
        m_camera = new Camera();
    }

    void Window::SetCameraAngle(float xoff, float yoff)
    {
        if (m_camera)
        {
            m_camera->ProcessMouseMovement(xoff, yoff);
        }
    }

    Camera* Window::GetCamera()
    {
        return m_camera;
    }

    glm::mat4 Window::GetProjectionMatrix()
    {
        return m_projection;
    }

    glm::mat4 Window::GetViewMatrix()
    {
        return m_view;
    }

    void Window::ReceiveGridData(GridData m_gridData)
    {
        m_gridLines = m_gridData.gridData;
        m_singleTileSize = m_gridData.tileSize;
    }

    void Window::PrepareRendering()
    {
        glGenVertexArrays(1, &lineVAO);
        glGenBuffers(1, &lineVBO);

        glBindVertexArray(lineVAO);

        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
        glBufferData(GL_ARRAY_BUFFER, m_gridLines.size() * sizeof(glm::vec3), m_gridLines.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        // UI VAO & VBO

        glGenVertexArrays(1, &crossHairVAO);
        glGenBuffers(1, &crossHairVBO);

        glBindVertexArray(crossHairVAO);
        glBindBuffer(GL_ARRAY_BUFFER, crossHairVBO);
        glBufferData(GL_ARRAY_BUFFER, m_crossHairLines.size() * sizeof(glm::vec3), m_crossHairLines.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

    }

    void Window::DrawGridLines()
    {
        //std::cout << "drawing lines..." << std::endl;
        glBindVertexArray(lineVAO);
        glDrawArrays(GL_LINES, 0, m_gridLines.size());
        //glBindVertexArray(0);
    }

    void Window::DrawUI()
    {
        glBindVertexArray(crossHairVAO);
        glDrawArrays(GL_LINES, 0, m_crossHairLines.size());
    }

    void Window::SetShaderData(Shader* shader)
    {
        shader->setMat4("view", GetViewMatrix());
        shader->setMat4("projection", GetProjectionMatrix());
        shader->setMat4("model", m_model);
        shader->setVec3("lineColor", glm::vec3(0.0f,0.0f,0.0f));
    }

    void Window::DestroyWindow()
    {
        if (!m_winIsClosed)
        {
            m_winIsClosed = true;
            glfwDestroyWindow(m_window);
            delete m_shaderPtr;
            m_shaderPtr = nullptr;
        }
    }

    bool Window::ShouldClose()
    {
        return glfwWindowShouldClose(m_window);
    }

    bool Window::IsClosed()
    {
        return m_winIsClosed;
    }

    bool Window::IsCurrent()
    {
        if (m_window == glfwGetCurrentContext())
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    unsigned int Window::GetWidth()
    {
        return m_winWidth;
    }

    unsigned int Window::GetHeight()
    {
        return m_winHeight;
    }

    const char* Window::GetTitle()
    {
        return m_winTitle;
    }
}

