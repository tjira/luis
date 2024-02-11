#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 position, normal = glm::vec3(0), color = glm::vec3(1);
};

class Buffer {
public:

    // Constructors and destructors
    Buffer(const Buffer& buffer) : data(buffer.getData()) { generate(); };
    Buffer(const std::vector<Vertex>& data) : data(data) { generate(); };
    Buffer() : data(0) { generate(); }; ~Buffer();

    // Operators
    Buffer& operator=(const Buffer& buffer);

    // Getters
    std::vector<Vertex> getData() const { return data; }
    size_t getSize() const { return data.size(); };

    // State functions
    void bind() const;

private:
    std::vector<Vertex> data;
    unsigned int vao, vbo;
    void generate();
};
