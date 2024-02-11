#include "ptable.h"
#include "gui.h"
#include <argparse/argparse.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <ImGuiFileDialog.h>

std::string vertex = R"(
#version 420 core
layout(location = 0) in vec3 i_position;
layout(location = 1) in vec3 i_normal;
layout(location = 2) in vec3 i_color;
uniform mat4 u_model, u_view, u_proj;
out vec3 fragment, normal, color;
out mat3 transform;
void main() {
    normal = normalize(mat3(transpose(inverse(u_model))) * i_normal);
    fragment = vec3(u_model * vec4(i_position, 1)), color = i_color;
    gl_Position = u_proj * u_view * vec4(fragment, 1);
    transform = inverse(mat3(u_view));
})";

std::string fragment = R"(
#version 420 core
struct Light { vec3 position; float ambient, diffuse, specular, shininess; };
uniform Light u_light; uniform vec3 u_camera;
in vec3 fragment, normal, color;
in mat3 transform;
out vec4 o_color;
void main() {
    vec3 lightPos = transform * u_light.position, reflection = reflect(-normalize(lightPos), normal), direction = normalize(u_camera - fragment);
    vec3 specular = vec3(pow(max(dot(direction, reflection), 0), u_light.shininess)),  diffuse = vec3(max(dot(normal, normalize(lightPos)), 0));
    o_color = vec4((vec3(u_light.ambient) + u_light.diffuse * diffuse + u_light.specular * specular), 1) * vec4(color, 1);
})";

std::string stencil = R"(
#version 420 core
out vec4 o_color;
void main() {
    o_color = vec4(1, 1, 1, 1);
})";

void keyCallback(GLFWwindow* window, int key, int, int action, int mods) {
    if (GLFWPointer* pointer = (GLFWPointer*)glfwGetWindowUserPointer(window); action == GLFW_PRESS) {
        if (mods == GLFW_MOD_CONTROL) {
            if (key == GLFW_KEY_E) {
                std::string files = "Molecule Files{.allxyz,.xyz},All Files{.*}";
                ImGuiFileDialog::Instance()->OpenDialog("Export Molecule", "Export Molecule", files.c_str(), "");
            } else if (key == GLFW_KEY_O) {
                std::string files = "Molecule Files{.allxyz,.xyz},All Files{.*}";
                ImGuiFileDialog::Instance()->OpenDialog("Import Molecule", "Import Molecule", files.c_str(), "");
            } else if (key == GLFW_KEY_Q) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            } else if (key == GLFW_KEY_S) {
                std::string files = "Image Files{.png,.jpg,.bmp},All Files{.*}";
                ImGuiFileDialog::Instance()->OpenDialog("Save Molecule", "Save Molecule", files.c_str(), "");
            }
        }
        else if (key == GLFW_KEY_F1) pointer->flags.options = !pointer->flags.options;
        else if (key == GLFW_KEY_F2) pointer->flags.system = !pointer->flags.system;
        else if (key == GLFW_KEY_F3) pointer->flags.ptable = !pointer->flags.ptable;
        else if (key == GLFW_KEY_F11) {
            static int xpos0, ypos0, width0, height0;
            int xpos, ypos, width, height;
            if (pointer->flags.fullscreen = !pointer->flags.fullscreen; pointer->flags.fullscreen) {
                glfwGetWindowSize(pointer->window, &width0, &height0);
                glfwGetWindowPos(pointer->window, &xpos0, &ypos0);
                glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &xpos, &ypos, &width, &height);
                glfwSetWindowMonitor(pointer->window, glfwGetPrimaryMonitor() , 0, 0, width, height, 1.0 / 60);
            } else {
                glfwSetWindowMonitor(pointer->window, nullptr , xpos0, ypos0, width0, height0, 1.0 / 60);
            }
        }
        else if (key == GLFW_KEY_F12) pointer->flags.info = !pointer->flags.info;
        else if (key == GLFW_KEY_SPACE) pointer->flags.pause = !pointer->flags.pause;
    }
}

void positionCallback(GLFWwindow* window, double x, double y) {
    GLFWPointer* pointer = (GLFWPointer*)glfwGetWindowUserPointer(window);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !ImGui::GetIO().WantCaptureMouse) {
        glm::vec3 xaxis = glm::inverse(glm::mat3(pointer->camera.view)) * glm::vec3(0, 1, 0);
        glm::vec3 yaxis = glm::inverse(glm::mat3(pointer->camera.view)) * glm::vec3(1, 0, 0);
        pointer->camera.view = glm::rotate(pointer->camera.view, 0.01f * ((float)y - pointer->mouse.y), yaxis);
        pointer->camera.view = glm::rotate(pointer->camera.view, 0.01f * ((float)x - pointer->mouse.x), xaxis);
    }
    pointer->mouse = { x, y };
}

void resizeCallback(GLFWwindow* window, int width, int height) {
    if (GLFWPointer* pointer = (GLFWPointer*)glfwGetWindowUserPointer(window); width > 0 && height > 0) {
        pointer->camera.proj = glm::perspective(glm::radians(45.0f), (float)width / height, 0.01f, 1000.0f);
        pointer->width = width, pointer->height = height; glViewport(0, 0, width, height);
    }
}

void scrollCallback(GLFWwindow* window, double, double dy) {
    if (!ImGui::GetIO().WantCaptureMouse) {
        ((GLFWPointer*)glfwGetWindowUserPointer(window))->camera.view *= glm::mat4(glm::mat3(1.0f + 0.08f * (float)dy));
    }
}

void set(const Shader& shader, const GLFWPointer::Camera& camera, const GLFWPointer::Light& light) {
    shader.use();
    shader.set<glm::vec3>("u_camera", -glm::inverse(glm::mat3(camera.view)) * glm::vec3(camera.view[3]));
    shader.set<glm::mat4>("u_view", camera.view);
    shader.set<glm::mat4>("u_proj", camera.proj);
    shader.set<glm::vec3>("u_light.position", light.position);
    shader.set<float>("u_light.shininess", light.shininess);
    shader.set<float>("u_light.specular", light.specular);
    shader.set<float>("u_light.ambient", light.ambient);
    shader.set<float>("u_light.diffuse", light.diffuse);
}

int main(int argc, char** argv) {
    // initialize the argument parser and container for the arguments
    argparse::ArgumentParser program("Luis", "1.0", argparse::default_arguments::none);

    // add options to the parser
    program.add_argument("input").help("Luis input file.").default_value(std::string(""));
    program.add_argument("-h").help("Display this help message and exit.").default_value(false).implicit_value(true);

    // extract the variables from the command line
    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error &error) {
        std::cerr << error.what() << std::endl << std::endl << program; return EXIT_FAILURE;
    }

    // print help if the help flag was provided
    if (program.get<bool>("-h")) {
        std::cout << program.help().str(); return EXIT_SUCCESS;
    }

    // Initialize GLFW and throw error if failed
    if(!glfwInit()) {
        throw std::runtime_error("Error during GLFW initialization.");
    }

    // Create GLFW variable struct
    GLFWPointer pointer; 

    // Pass OpenGL version and other hints
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, pointer.major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, pointer.minor);
    glfwWindowHint(GLFW_SAMPLES, pointer.samples);

    // Create the window
    if (pointer.window = glfwCreateWindow(pointer.width, pointer.height, pointer.title.c_str(), nullptr, nullptr); !pointer.window) {
        throw std::runtime_error("Error during window creation.");
    }

    // Initialize GLAD
    if (glfwMakeContextCurrent(pointer.window); !gladLoadGL(glfwGetProcAddress)) {
        throw std::runtime_error("Error during GLAD initialization.");
    }

    // Enable some options
    glEnable(GL_DEPTH_TEST), glEnable(GL_CULL_FACE), glEnable(GL_STENCIL_TEST);
    glfwSetWindowUserPointer(pointer.window, &pointer);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glfwSwapInterval(1);

    // Set event callbacks
    glfwSetCursorPosCallback(pointer.window, positionCallback);
    glfwSetWindowSizeCallback(pointer.window, resizeCallback);
    glfwSetScrollCallback(pointer.window, scrollCallback);
    glfwSetKeyCallback(pointer.window, keyCallback);

    // Initialize camera matrices
    pointer.camera.proj = glm::perspective(glm::radians(45.0f), (float)pointer.width / pointer.height, 0.01f, 1000.0f);
    pointer.camera.view = glm::lookAt({ 0.0f, 0.0f, 5.0f }, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    {
        // Initialize meshes
        for (auto& [symbol, object] : ptable) {
            Geometry::meshes[symbol] = Mesh::Icosphere(SUBDIVISIONS, SMOOTH, symbol);
            Geometry::meshes.at(symbol).setColor(object.color);
        }
        Geometry::meshes["bond"] = Mesh::Cylinder(SECTORS, SMOOTH, "bond"); 

        // Create scene, shader and GUI
        Trajectory trajectory;
        if (!program.get<std::string>("input").empty()) {
            trajectory = Trajectory::Load(program.get<std::string>("input"));
        }
        Shader shader(vertex, fragment);
        Shader sshader(vertex, stencil);
        Gui gui(pointer.window);
        
        // Enter the render loop
        while (!glfwWindowShouldClose(pointer.window)) {
            
            // Clear the color and depth buffer
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            // Set shader variables
            set(sshader, pointer.camera, pointer.light);
            set(shader, pointer.camera, pointer.light);

            // Pause or unpause the trajectory
            trajectory.getPause() = pointer.flags.pause;

            // Render the mesh and GUI
            trajectory.render(shader, sshader, pointer.highlight);
            gui.render(trajectory);
            
            // Swap buffers and poll events
            glfwSwapBuffers(pointer.window);
            glfwPollEvents();
        }
    }

    // Clean up generated meshes and terminate GLFW
    Geometry::meshes.clear(); glfwTerminate();
}
