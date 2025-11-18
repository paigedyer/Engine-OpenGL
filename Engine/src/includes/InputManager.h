//
// Created by Paige on 11/12/2025.
//

#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#define GLFW_INCLUDE_NONE

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

#include "InputManager.h"

class Input
{
public:
    Input();

    struct MouseButton
    {
        int button;
        int action;
    };


private:

};

#endif //INPUTMANAGER_H
