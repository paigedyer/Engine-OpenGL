#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#define GLFW_INCLUDE_NONE

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <Camera.h>
#include <vector>
#include <SHADER.h>

#include <InputManager.h>

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600
#define DEFAULT_TITLE "Iso Engine Window"

namespace WindowManager
{

    class Window
    {
    public:
            // Stores the grid data passed from the tilemap editor application
        struct GridData
        {
            std::vector<glm::vec3> gridData;
            glm::vec3 tileSize;
            float numCols;
            float numRows;
        };

            // Constructor
        Window(unsigned int width=DEFAULT_WINDOW_WIDTH,
            unsigned int height=DEFAULT_WINDOW_HEIGHT,
            const char *title=DEFAULT_TITLE,
            GLFWwindow* winContext=NULL);

        ///////////// WINDOW UTILITY FUNCTIONS ///////////////////

        void SetFocused();
        void TransformScreen(float x, float y);
        void ZoomScreen(float zoom);

        //////////////// INITIALIZING FUNCTIONS ///////////////////////////

        static bool Init();
        void PrepareRendering();
        void SetFramebufferValues(int width, int height);

        ///////////////// SHUTDOWN FUNCTIONS //////////////////////////

        void DestroyWindow();

        ////////////// CAMERA FUNCTIONALITY ////////////////////////

        Camera* GetCamera();
        void SetCameraAngle(float xoff, float yoff);
        glm::mat4 GetProjectionMatrix();
        glm::mat4 GetViewMatrix();

        ////////////////// UPDATE FUNCTIONS /////////////////////////

        void onUpdate();

        ////////////// IMGUI COMMANDS ////////////////
        void UICommands();

        ///////// FUNCTIONS FOR THE TILEMAP EDITOR /////////////////////

        void ReceiveGridData(GridData gridData);

        //////////// GET INFORMATION ABOUT WINDOW //////////////////

        bool ShouldClose();
        bool IsClosed();
        bool IsCurrent();
        const char* GetTitle();
        unsigned int GetWidth();
        unsigned int GetHeight();
        GLFWwindow* GetWindow();


    private:

        //////////////// INITIALIZING FUNCTIONS ///////////////////////////
        bool CreateWindow();
        void SetShaderData(Shader* shader);

        ////////////// CAMERA FUNCTIONALITY ////////////////////////

        glm::vec3 UnProject(glm::vec3 inVec3);
        glm::vec3 Project(glm::vec3 inVec3);
        void SetupCamera();

        ////////////////// UPDATE FUNCTIONS /////////////////////////

        void Update();
        void CalculateDeltaTime();
        void Clear();

        ///////// FUNCTIONS FOR THE TILEMAP EDITOR /////////////////////

        void DrawGridLines();
        void DrawUI();

        glm::vec3 m_lastScreenCoordinates = glm::vec3(0.0f);
        glm::vec3 m_currentCoords = glm::vec3(0.0f);

        unsigned int m_winWidth = DEFAULT_WINDOW_WIDTH;
        unsigned int m_winHeight = DEFAULT_WINDOW_HEIGHT;

        const char* m_winTitle = DEFAULT_TITLE;

        /// IMGUI ///
        ImGuiIO* m_io;

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

        // Variables for working with IMGUI - TEMPORARY
        float m_gridx = 0.0f;
        float m_gridy = 0.0f;
    };

    ///////////////// SHUTDOWN FUNCTIONS //////////////////////////
    void CloseGLFW();

}

#endif
