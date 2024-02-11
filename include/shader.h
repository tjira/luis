#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <stdexcept>
#include <vector>

class Shader {
public:

    // Constructors and destructors
    ~Shader(); Shader(const std::string& vertex, const std::string& fragment);

    // Setters
    template <typename T> void set(const std::string& name, T value) const;

    // State functions
    void use() const;

private:
    void errorCheck(unsigned int shader, const std::string& title) const;
    unsigned int id;
};
