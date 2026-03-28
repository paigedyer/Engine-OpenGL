#ifndef GRIDCOORDINATE_MANAGER_H
#define GRIDCOORDINATE_MANAGER_H

#define GLFW_INCLUDE_NONE
#include <vector>

namespace WindowManager
{
    class GridCoordinates;
    class UIElement
    // A window or section of the UI. Contains information needed to work within only one individual element
    {
        GridCoordinates* gridCoordinates;
        std::vector<UIElement> elements;
    public:

        UIElement();
        void RunElements();
        void InitializeUIElements();

        virtual void Run();
        void SetupNewElement();

        void GetGridPosFromWindow(float x, float y);

    protected:
        void InitGridCoords();
    };

    class GridCoordinates : public UIElement
    {

    public:
        GridCoordinates(float x=0.0f, float y=0.0f, const char *name = "GridCoordinates");

        const char* m_name = NULL;
        float m_gridx;
        float m_gridy;

        void Run() override;

        void SetupNewElement();

        glm::vec2 GetGridValues();
        void SetGridValues(float x, float y);

    };;
}
#endif