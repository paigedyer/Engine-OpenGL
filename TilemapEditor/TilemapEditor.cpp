#include <vector>
#include <glad/glad.h>
#include <WindowManager.h>
#include <SHADER.h>

#include <TilemapEditor.h>

namespace TilemapEditor
{
    GLFWwindow* currentWindow = nullptr;

    void OpenWindows()
    {
        if (WindowManager::Window::Init())
        {
            // generates a grid
            Grid grid = Grid();
            std::cout << grid.GetNumLines() << std::endl;

            glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);

            //WindowManager::Window paletteWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "Palette");
            // Share context between the two windows, to share textures, etc.
            WindowManager::Window editorWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "Tilemap Editor");

            editorWindow.SetFocused();
            currentWindow = editorWindow.GetWindow();

            // Send the generated grid data to the editor window
            editorWindow.ReceiveGridData(grid.GetGridData());
            editorWindow.PrepareRendering();

            // RENDER LOOP ENTRY
            while (!editorWindow.IsClosed())
            {
                if (editorWindow.ShouldClose()) {editorWindow.DestroyWindow();}
                else
                {
                    if (editorWindow.IsCurrent())
                    {
                        editorWindow.onUpdate();
                    }
                }
            }
            if (editorWindow.IsClosed()) { WindowManager::CloseGLFW(); }
        }
    }

    Grid::Grid()
    {
        GenerateGrid();
    }

    int Grid::GetNumLines()
    {
        return m_gridLines.size();
    }

    void Grid::GenerateGrid()
    {
        std::cout << "Generating grid..." << std::endl;
        // Vertical grid lines
        for (int x = 0; x< m_numCols; ++x)
        {
            float xpos = x * m_tileSize;
            m_gridLines.push_back(glm::vec3(xpos, 0.0f, 0.0f));
            m_gridLines.push_back(glm::vec3(xpos, 0.0f, (m_numRows * m_tileSize)));
        }

        // Horizontal grid lines
        for (int z = 0; z < m_numRows; ++z)
        {
            float zpos = z * m_tileSize;
            m_gridLines.push_back(glm::vec3(0.0f, 0.0f, zpos));
            m_gridLines.push_back(glm::vec3(m_numCols * m_tileSize, 0.0f, zpos));
        }

    }

    WindowManager::Window::GridData Grid::GetGridData()
    {
        m_gridData.gridData = m_gridLines;
        m_gridData.tileSize = glm::vec3(m_tileSize, 0.0f, m_tileSize);
        m_gridData.numCols = m_numCols;
        m_gridData.numRows = m_numRows;
        return m_gridData;
    }

    Tile::Tile()
    {
        InitializeTile();
    }

    void Tile::InitializeTile()
    {

    }

}
