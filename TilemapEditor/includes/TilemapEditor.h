#ifndef TILEMAPEDITOR_H
#define TILEMAPEDITOR_H

#define GLFW_INCLUDE_NONE

#define DEFAULT_TILE_SIZE 5.0f
#define DEFAULT_NUM_COLS 50
#define DEFAULT_NUM_ROWS 50

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <WindowManager.h>
#include <SHADER.h>
#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>

namespace TilemapEditor
{
    void OpenWindows();
    bool ExitProgram();

    void PaletteWindowRenderCommands();

    class Grid
        {

        public:
            Grid();
            void GenerateGrid();
            WindowManager::Window::GridData GetGridData();
            int GetNumLines();
        private:
            float m_tileSize = DEFAULT_TILE_SIZE;
            unsigned int m_numRows = DEFAULT_NUM_ROWS;
            unsigned int m_numCols = DEFAULT_NUM_COLS;
            std::vector<glm::vec3> m_gridLines;
            WindowManager::Window::GridData m_gridData;


        };
    class Tile
        {
        public:
            Tile();
        private:
            void InitializeTile();

        };
}


#endif
