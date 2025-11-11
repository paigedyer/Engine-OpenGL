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
        struct GridData
        {
            std::vector<glm::vec3> gridData;
            glm::vec3 tileSize;
            float numCols;
            float numRows;
        };

        Window(unsigned int width=DEFAULT_WINDOW_WIDTH,
            unsigned int height=DEFAULT_WINDOW_HEIGHT,
            const char *title=DEFAULT_TITLE,
            GLFWwindow* winContext=NULL);

        void SetFocused();
        void SetFramebufferValues(int width, int height);

        void onUpdate();
        void PrepareRendering();
        void TransformScreen(float x, float y);
        void ZoomScreen(float zoom);

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

        void ReceiveGridData(GridData gridData);

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
        glm::vec3 UnProject(glm::vec3 inVec3);
        glm::vec3 Project(glm::vec3 inVec3);

        glm::vec3 m_lastScreenCoordinates = glm::vec3(0.0f);
        glm::vec3 m_currentCoords = glm::vec3(0.0f);

        unsigned int m_winWidth = DEFAULT_WINDOW_WIDTH;
        unsigned int m_winHeight = DEFAULT_WINDOW_HEIGHT;

        const char* m_winTitle = DEFAULT_TITLE;

        bool m_winIsClosed = false;
        bool m_firstFrame = true;

        double m_frameRate = 60.0;
        double m_fixedDeltaTime = 1.0 / m_frameRate;
        double m_deltaTime, m_lastUpdate = 0.0;
        double m_lastFrameTime = 0.0;

        GLFWwindow* m_winContext = NULL;
        GLFWwindow* m_window = NULL;

        Camera* m_camera;

        // Position in grid coordinates
        glm::vec3 m_gridOffset = glm::vec3(0.0f);
        // For resuse of rotation and scale
        glm::mat4 m_isoMatrix;
        // Inverse for unprojecting screen deltas
        glm::mat4 m_invIsoMatrix;

        glm::vec3 m_worldOrigin = glm::vec3(0.0f, 0.0f, 0.0f);

        glm::vec3 m_deltaScreenPos = glm::vec3(0.0f);
        glm::vec3 m_currentScreenPos = glm::vec3(0.0f, 0.0f, 0.0f);

        glm::vec3 m_singleTileSize;

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

        std::vector<glm::vec3> m_gridLines;
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
