#pragma once

#include "trajectory.h"
#include <GLFW/glfw3.h>
#include <ImGuiFileDialog.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <implot.h>

class Gui {
public:
    Gui(GLFWwindow* window); ~Gui();
    void render(Trajectory& movie);

private:
    GLFWwindow* window;
};
