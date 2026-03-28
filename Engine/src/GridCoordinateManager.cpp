#define GLFW_INCLUDE_NONE
#include <glm/glm.hpp>

#include <WindowManager.h>
#include <GridCoordinateManager.h>
#include "Engine.h"

namespace WindowManager
{
    UIElement::UIElement()
    {

    }
    void UIElement::RunElements()
    {
        gridCoordinates->Run();

    }

    void UIElement::SetupNewElement()
    {
        std::cout << "Setting up stuff" << std::endl;
    }

    void UIElement::InitializeUIElements()
    {
        // Initialize IMGUI
        ImGui::GetCurrentContext();
        gridCoordinates = new GridCoordinates(0.0f, 0.0f, "Grid Coordinates");
    }

    void UIElement::GetGridPosFromWindow(float x, float y)
    {
        gridCoordinates->SetGridValues(x, y);
    }

    void UIElement::InitGridCoords() {}

    void UIElement::Run()
    {
    }

    ////////// GRID COORDINATES //////////

    GridCoordinates::GridCoordinates(float x, float y, const char *name) : m_gridx(x), m_gridy(y), m_name(name)
    {

    }


    void GridCoordinates::Run()
    {
        std::cout << "GRID RUNNING" << std::endl;
        ImGui::Begin(m_name);

        ImGui::SeparatorText("separator text here");
        ImGui::PushItemWidth(100);
        ImGui::InputFloat("X", &m_gridx, 0.01f, 1.0f, "%.3f"); ImGui::SameLine();
        ImGui::InputFloat("Y", &m_gridy, 0.01f, 1.0f, "%.3f");
        ImGui::PopItemWidth();

        ImGui::Text("Hello this is some testing text to see if the word wrapping is working horray!!!!");
        ImGui::End();
    }

    glm::vec2 GridCoordinates::GetGridValues()
    {
        return glm::vec2(m_gridx, m_gridy);
    }

    void GridCoordinates::SetGridValues(float x, float y)
    {
        m_gridx = x;
        m_gridy = y;
    }

}