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

        void SetFocused();

        void onUpdate();
        void PrepareRendering();
        void TransformScreen(float x, float y);

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
        void DrawUI();
        void SetShaderData(Shader* shader);
        glm::vec3 CalculateTranslation();

        unsigned int m_winWidth = DEFAULT_WINDOW_WIDTH;
        unsigned int m_winHeight = DEFAULT_WINDOW_HEIGHT;

        const char* m_winTitle = DEFAULT_TITLE;

        bool m_winIsClosed = false;
        bool m_firstFrame = true;

        float m_deltaTime, lastFrame = 0.0f;

        GLFWwindow* m_winContext = NULL;
        GLFWwindow* m_window = NULL;

        Camera* m_camera;

        glm::vec3 m_newTranslation = glm::vec3(0.0f,0.0f,0.0f);
        glm::vec3 m_currentTranslation = glm::vec3(-25.0f, 0.0f, -25.0f);

        // SHADER & RENDERING DATA
            // View matrix
        glm::mat4 m_view;
            // Projection matrix
        float m_zoom = 1.0f;
        glm::mat4 m_projection;
            // Model matrix (grid)
        glm::mat4 m_model;

            // Shader program pointers
        Shader* m_shaderPtr;
        Shader* m_uiShaderPtr;

        std::vector<glm::vec3> m_gridData;
        GLuint lineVBO;
        GLuint lineVAO;
        GLuint crossHairVAO;
        GLuint crossHairVBO;

        std::vector<glm::vec3> m_crossHairLines =
        {
            glm::vec3(-0.02f,  0.0f, 0.0f), // Horizontal left
            glm::vec3( 0.02f,  0.0f, 0.0f), // Horizontal right
            glm::vec3( 0.0f, -0.02f, 0.0f), // Vertical bottom
            glm::vec3( 0.0f,  0.02f, 0.0f), // Vertical top
        };
    };

    void CloseGLFW();
}

#endif
