#pragma once

#include "geometry.h"
#include <chrono>
#include <fstream>
#include <sstream>

class Trajectory {
public:
    
    // Constructors
    Trajectory() {}

    // Static constructors
    static Trajectory Load(const std::string& movie);

    // Getters
    std::vector<Geometry>& getGeoms() { return geoms; }
    bool& getPause() { return paused; }
    int& getFrame() { return frame; }
    float& getWait() { return wait; }
    int size() const { return geoms.size(); }

    // State functions
    void moveBy(const glm::vec3& vector);
    void render(const Shader& shader, const Shader& sshader, int highlight);

private:
    std::chrono::high_resolution_clock::time_point timestamp;
    std::vector<Geometry> geoms;
    bool paused = false;
    float wait = 15.997;
    int frame = 0;
};
