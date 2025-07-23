#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <Camera.h>
#include <vector>
#include <SHADER.h>

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600
#define DEFAULT_TITLE "Iso Engine Window"

namespace WindowManager
{
    class Window
    {
    public:
        Window(unsigned int width=DEFAULT_WINDOW_WIDTH,
            unsigned int height=DEFAULT_WINDOW_HEIGHT,
            const char *title=DEFAULT_TITLE,
            GLFWwindow* winContext=NULL);

        void onUpdate();
        void PrepareRendering();

        bool ShouldClose();
        bool IsClosed();
        bool IsCurrent();

        static bool Init();

        const char* GetTitle();

        unsigned int GetWidth();
        unsigned int GetHeight();

        Camera* GetCamera();
        void SetCameraAngle(float xoff, float yoff);

        glm::mat4 GetProjectionMatrix();
        glm::mat4 GetViewMatrix();

        void ReceiveGridData(std::vector<glm::vec3> m_gridLines);

        GLFWwindow* GetWindow();

        void DestroyWindow();
    private:
        bool CreateWindow();
        void Update();
        void Clear();
        void SetupCamera();
        void ProcessInput();
        void CalculateDeltaTime();
        void DrawGridLines();
        void SetShaderData(Shader* shader);

        unsigned int m_winWidth = DEFAULT_WINDOW_WIDTH;
        unsigned int m_winHeight = DEFAULT_WINDOW_HEIGHT;

        const char* m_winTitle = DEFAULT_TITLE;

        bool m_winIsClosed = false;

        float m_deltaTime, lastFrame = 0.0f;

        GLFWwindow* m_winContext = NULL;
        GLFWwindow* m_window = NULL;

        // View matrix
        glm::mat4 m_view;
        // Projection matrix
        glm::mat4 m_projection;

        glm::mat4 m_model;

        Camera* m_camera;

        Shader* m_shaderPtr;

        std::vector<glm::vec3> m_gridData;
        GLuint lineVBO;
        GLuint lineVAO;
    };

    void CloseGLFW();
}

#endif
