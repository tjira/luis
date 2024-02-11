#pragma once

#include "ptable.h"
#include "glfwpointer.h"
#include "mesh.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <unordered_map>

class Geometry {
    struct Object {
        glm::mat4 getModel(glm::mat4 s = glm::mat4(1)) const {
            return translate * rotate * s * scale;
        }
        glm::vec3 getPosition() const {
            return glm::vec3(translate[3]);
        }
        glm::mat4 translate, rotate, scale;
        std::string name;
    };

public:

    // Constructors
    Geometry() {};

    // Statc constructors
    static Geometry Load(std::stringstream& file);

    // Getters
    std::vector<Object>& getObjects();
    glm::vec3 getCenter() const;
    size_t size() const;

    // Setters
    void setAtomSizeFactor(float factor);
    void setBondSize(float size);

    // State functions
    void moveBy(const glm::vec3& vector);
    void render(const Shader& shader, const Shader& sshader, int highlight = -1) const;
    void rebind(float factor);

    // Public static variables
    inline static std::unordered_map<std::string, Mesh> meshes;

private:
    std::vector<Object> objects;
};
