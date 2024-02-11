#include "trajectory.h"

/*
Function that loads an xyz file with molecular trajectory.
*/
Trajectory Trajectory::Load(const std::string& filename) {

    // Create the graphiv trajectory object
    Trajectory trajectory;

    // Open a file and create buffer
    std::ifstream file(filename); std::string line;
    std::getline(file, line);

    // Get the atom count (length) and file length (block).
    int length = std::stoi(line), block = 1;
    while (std::getline(file, line)) block++;

    // Create the vector of geometris and reset the file reader.
    trajectory.geoms.resize(block / (length + 2));
    file.clear(), file.seekg(0);

    // Read the individual geometries.
    for (int i = 0; i < block / (length + 2); i++) {
        std::stringstream ss;

        // For lines in one geometry.
        for (int j = 0; j < length + 2; j++) {
            std::getline(file, line); ss << (j ? "\n" : "" ) << line;
        }

        // Load a geometry to a molecule class.
        trajectory.geoms.at(i) = Geometry::Load(ss);
    }

    // Set the initialization timestamp (for FPS manipulation).
    trajectory.timestamp = std::chrono::high_resolution_clock().now();

    // Center the trajectory
    trajectory.moveBy(-trajectory.geoms.at(0).getCenter());

    // Return the trajectory
    return trajectory;
}

/*
Move the trajectory by some vector.
*/
void Trajectory::moveBy(const glm::vec3& vector) {
    for (Geometry& geom : geoms) geom.moveBy(vector);
}

/*
Renders the trajectory with the provided shader.
*/
void Trajectory::render(const Shader& shader, const Shader& sshader, int highlight) {
    if (geoms.size()) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock().now() - timestamp).count();
        if (elapsed > wait) {
            if (!paused && wait > 0) frame = (frame + (int)(elapsed / wait)) % (int)geoms.size();
            timestamp = std::chrono::high_resolution_clock().now();
        }
        geoms.at(frame).render(shader, sshader, highlight);
    }
}
