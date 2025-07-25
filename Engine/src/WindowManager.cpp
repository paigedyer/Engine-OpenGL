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

    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        // Viewport adjusts to new size
        glViewport(0, 0, width, height);
    }

    bool firstMouse = true;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float lastX = 0.0f;
    float lastY = 0.0f;
    float fov = 45.0f;

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

    void Window::SetFocused()
    {
        glfwFocusWindow(m_window);
    }

    void Window::TransformScreen(float x, float z)
    {
        float scale = 0.08;
        m_newTranslation.x += z * scale;
        m_newTranslation.z += x * scale;
        /*
        if (x == 0.0f)
        {
            m_newTranslation.x = 0.0f;
        }
        if (y == 0.0f)
        {
            m_newTranslation.y = 0.0f;
        }
        if (x > 0.0f)
        {
            m_newTranslation.x = 2.0f;
        }
        if (y > 0.0f)
        {
            m_newTranslation.y = 1.0f;
        }
        if (y < 0.0f)
        {
            m_newTranslation.y = -1.0f;
        }
        if (x < 0.0f)
        {
            m_newTranslation.x = -2.0f;
        }
        */
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

        SetupCamera();

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
        ProcessInput();
        Clear();

        m_shaderPtr->Use();

        m_model = glm::mat4(1.0f);
        m_model = glm::scale(m_model, glm::vec3(0.5f, 1.0f, 1.0f));

        m_currentTranslation += m_newTranslation;
        std::cout << "X: " << m_currentTranslation.x << std::endl;
        std::cout << "Z: " << m_currentTranslation.z << std::endl;
        m_model = glm::translate(m_model, m_currentTranslation);
        //m_model = glm::translate(m_model, (glm::vec3(-25.0f, 0.0f, -25.0f) * 5.0f)); // Confirmed good position
        m_model = glm::rotate(m_model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        if (m_camera)
        {
            m_zoom = 0.08;
            //m_projection = glm::perspective(glm::radians(45.0f), (float)m_winWidth / (float)m_winHeight, 0.1f, 1000.0f);
            m_projection = glm::ortho(
                (-(m_winWidth / 2.0f) * m_zoom),
                ((m_winWidth / 2.0f) * m_zoom),
                ((m_winHeight / 2.0f) * m_zoom),
                (-(m_winHeight / 2.0f) * m_zoom),
                (200.0f * 5),
                (-200.0f * 5)
                );
            m_view = m_camera->GetViewMatrix();
        }
        else
        {
            std::cout << "Camera is NULL" << std::endl;
        }

        SetShaderData(m_shaderPtr);

        // vvv Draw calls vvv
        glLineWidth(1.0f);
        DrawGridLines();
            // UI/HUD
        glLineWidth(2.0f);
        m_uiShaderPtr->Use();
        DrawUI();

        m_newTranslation = glm::vec3(0.0f);

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    void Window::onUpdate()
    {
        Update();
    }

    void Window::CalculateDeltaTime()
    {
        float currentFrame = glfwGetTime();
        m_deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }

    void Window::ProcessInput()
    {
        if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
        {
            if (m_camera)
            {
                m_camera->ProcessKeyboard(FORWARD, m_deltaTime);
            }
        }
        if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        {
            if (m_camera)
            {
                m_camera->ProcessKeyboard(BACKWARD, m_deltaTime);
            }
        }
        if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        {
            if (m_camera)
            {
                m_camera->ProcessKeyboard(LEFT, m_deltaTime);
            }
        }
        if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        {
            if (m_camera)
            {
                m_camera->ProcessKeyboard(RIGHT, m_deltaTime);
            }
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

    void Window::ReceiveGridData(std::vector<glm::vec3> m_gridLines)
    {
        m_gridData = m_gridLines;
    }

    void Window::PrepareRendering()
    {
        glGenVertexArrays(1, &lineVAO);
        glGenBuffers(1, &lineVBO);

        glBindVertexArray(lineVAO);

        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
        glBufferData(GL_ARRAY_BUFFER, m_gridData.size() * sizeof(glm::vec3), m_gridData.data(), GL_STATIC_DRAW);

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
        glDrawArrays(GL_LINES, 0, m_gridData.size());
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

