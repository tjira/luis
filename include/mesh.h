#pragma once

#include "buffer.h"
#include "shader.h"
#include <algorithm>

class Mesh {
public:

    // Constructors
    Mesh(std::vector<Vertex> data, const std::string& name = "mesh") : name(name), model(1.0f), buffer(data) {};
    Mesh() {};

    // Static constructors
    static Mesh Cylinder(int sectors, bool smooth, const std::string& name = "cylinder");
    static Mesh Icosphere(int subdivisions, bool smooth, const std::string& name = "icosphere");

    // Getters
    std::string getName() const; glm::vec3 getPosition() const;

    // Setters
    void setColor(const glm::vec3& color);
    void setModel(const glm::mat4& model);

    // State functions
    void render(const Shader& shader, const glm::mat4& transform = glm::mat4(1.0f)) const;

private:
    std::string name;
    glm::mat4 model;
    Buffer buffer;
};
