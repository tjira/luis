#include "mesh.h"

Mesh Mesh::Cylinder(int sectors, bool smooth, const std::string& name) {
    std::vector<Vertex> data;
    for (int j = 0; j < sectors; j++) {
        data.push_back({{ cosf(2 * (float)M_PI / sectors * (j + 0)),  1, sinf(2 * (float)M_PI / sectors * (j + 0)) }});
        data.push_back({{ cosf(2 * (float)M_PI / sectors * (j + 1)),  1, sinf(2 * (float)M_PI / sectors * (j + 1)) }});
        data.push_back({{ cosf(2 * (float)M_PI / sectors * (j + 1)), -1, sinf(2 * (float)M_PI / sectors * (j + 1)) }});

        data.push_back({{ cosf(2 * (float)M_PI / sectors * (j + 0)),  1, sinf(2 * (float)M_PI / sectors * (j + 0)) }});
        data.push_back({{ cosf(2 * (float)M_PI / sectors * (j + 1)), -1, sinf(2 * (float)M_PI / sectors * (j + 1)) }});
        data.push_back({{ cosf(2 * (float)M_PI / sectors * (j + 0)), -1, sinf(2 * (float)M_PI / sectors * (j + 0)) }});
    }
    for (size_t i = 0; i < data.size(); i += 3) {
        glm::vec3 v1 = data.at(i + 1).position - data.at(i).position;
        glm::vec3 v2 = data.at(i + 2).position - data.at(i).position;
        data.at(i + 0).normal = smooth ? data.at(i + 0).position : glm::normalize(glm::cross(v1, v2));
        data.at(i + 1).normal = smooth ? data.at(i + 1).position : glm::normalize(glm::cross(v1, v2));
        data.at(i + 2).normal = smooth ? data.at(i + 2).position : glm::normalize(glm::cross(v1, v2));
    }
    return Mesh(data, name);
}

Mesh Mesh::Icosphere(int subdivisions, bool smooth, const std::string& name) {
    std::vector<Vertex> data; float k = (1.0f + sqrtf(5.0f)) / 2.0f;

    data.push_back({glm::normalize(glm::vec3{ -1,  k,  0 })});
    data.push_back({glm::normalize(glm::vec3{ -k,  0,  1 })});
    data.push_back({glm::normalize(glm::vec3{  0,  1,  k })});

    data.push_back({glm::normalize(glm::vec3{ -1,  k,  0 })});
    data.push_back({glm::normalize(glm::vec3{  0,  1,  k })});
    data.push_back({glm::normalize(glm::vec3{  1,  k,  0 })});

    data.push_back({glm::normalize(glm::vec3{ -1,  k,  0 })});
    data.push_back({glm::normalize(glm::vec3{  1,  k,  0 })});
    data.push_back({glm::normalize(glm::vec3{  0,  1, -k })});

    data.push_back({glm::normalize(glm::vec3{ -1,  k,  0 })});
    data.push_back({glm::normalize(glm::vec3{  0,  1, -k })});
    data.push_back({glm::normalize(glm::vec3{ -k,  0, -1 })});

    data.push_back({glm::normalize(glm::vec3{ -1,  k,  0 })});
    data.push_back({glm::normalize(glm::vec3{ -k,  0, -1 })});
    data.push_back({glm::normalize(glm::vec3{ -k,  0,  1 })});

    data.push_back({glm::normalize(glm::vec3{  1,  k,  0 })});
    data.push_back({glm::normalize(glm::vec3{  0,  1,  k })});
    data.push_back({glm::normalize(glm::vec3{  k,  0,  1 })});

    data.push_back({glm::normalize(glm::vec3{  0,  1,  k })});
    data.push_back({glm::normalize(glm::vec3{ -k,  0,  1 })});
    data.push_back({glm::normalize(glm::vec3{  0, -1,  k })});

    data.push_back({glm::normalize(glm::vec3{ -k,  0,  1 })});
    data.push_back({glm::normalize(glm::vec3{ -k,  0, -1 })});
    data.push_back({glm::normalize(glm::vec3{ -1, -k,  0 })});

    data.push_back({glm::normalize(glm::vec3{ -k,  0, -1 })});
    data.push_back({glm::normalize(glm::vec3{  0,  1, -k })});
    data.push_back({glm::normalize(glm::vec3{  0, -1, -k })});

    data.push_back({glm::normalize(glm::vec3{  0,  1, -k })});
    data.push_back({glm::normalize(glm::vec3{  1,  k,  0 })});
    data.push_back({glm::normalize(glm::vec3{  k,  0, -1 })});

    data.push_back({glm::normalize(glm::vec3{  0, -1,  k })});
    data.push_back({glm::normalize(glm::vec3{  k,  0,  1 })});
    data.push_back({glm::normalize(glm::vec3{  0,  1,  k })});

    data.push_back({glm::normalize(glm::vec3{ -1, -k,  0 })});
    data.push_back({glm::normalize(glm::vec3{  0, -1,  k })});
    data.push_back({glm::normalize(glm::vec3{ -k,  0,  1 })});

    data.push_back({glm::normalize(glm::vec3{  0, -1, -k })});
    data.push_back({glm::normalize(glm::vec3{ -1, -k,  0 })});
    data.push_back({glm::normalize(glm::vec3{ -k,  0, -1 })});

    data.push_back({glm::normalize(glm::vec3{  k,  0, -1 })});
    data.push_back({glm::normalize(glm::vec3{  0, -1, -k })});
    data.push_back({glm::normalize(glm::vec3{  0,  1, -k })});

    data.push_back({glm::normalize(glm::vec3{  k,  0,  1 })});
    data.push_back({glm::normalize(glm::vec3{  k,  0, -1 })});
    data.push_back({glm::normalize(glm::vec3{  1,  k,  0 })});

    data.push_back({glm::normalize(glm::vec3{  1, -k,  0 })});
    data.push_back({glm::normalize(glm::vec3{  k,  0,  1 })});
    data.push_back({glm::normalize(glm::vec3{  0, -1,  k })});

    data.push_back({glm::normalize(glm::vec3{  1, -k,  0 })});
    data.push_back({glm::normalize(glm::vec3{  0, -1,  k })});
    data.push_back({glm::normalize(glm::vec3{ -1, -k,  0 })});

    data.push_back({glm::normalize(glm::vec3{  1, -k,  0 })});
    data.push_back({glm::normalize(glm::vec3{ -1, -k,  0 })});
    data.push_back({glm::normalize(glm::vec3{  0, -1, -k })});

    data.push_back({glm::normalize(glm::vec3{  1, -k,  0 })});
    data.push_back({glm::normalize(glm::vec3{  0, -1, -k })});
    data.push_back({glm::normalize(glm::vec3{  k,  0, -1 })});

    data.push_back({glm::normalize(glm::vec3{  1, -k,  0 })});
    data.push_back({glm::normalize(glm::vec3{  k,  0, -1 })});
    data.push_back({glm::normalize(glm::vec3{  k,  0,  1 })});

    for (int i = 0; i < subdivisions; i++) {
        std::vector<Vertex> subdivided;
        for (size_t j = 0; j < data.size(); j += 3) {
            glm::vec3 p1 = data.at(j + 0).position;
            glm::vec3 p2 = data.at(j + 1).position;
            glm::vec3 p3 = data.at(j + 2).position;
            glm::vec3 p4 = glm::normalize((p1 + p2) / 2.0f);
            glm::vec3 p5 = glm::normalize((p2 + p3) / 2.0f);
            glm::vec3 p6 = glm::normalize((p3 + p1) / 2.0f);

            subdivided.push_back({p1});
            subdivided.push_back({p4});
            subdivided.push_back({p6});

            subdivided.push_back({p4});
            subdivided.push_back({p2});
            subdivided.push_back({p5});

            subdivided.push_back({p6});
            subdivided.push_back({p5});
            subdivided.push_back({p3});

            subdivided.push_back({p4});
            subdivided.push_back({p5});
            subdivided.push_back({p6});
        }
        data = subdivided;
    }
    for (size_t i = 0; i < data.size(); i += 3) {
        glm::vec3 v1 = data.at(i + 1).position - data.at(i).position;
        glm::vec3 v2 = data.at(i + 2).position - data.at(i).position;
        data.at(i + 0).normal = smooth ? data.at(i + 0).position : glm::normalize(glm::cross(v1, v2));
        data.at(i + 1).normal = smooth ? data.at(i + 1).position : glm::normalize(glm::cross(v1, v2));
        data.at(i + 2).normal = smooth ? data.at(i + 2).position : glm::normalize(glm::cross(v1, v2));
    }
    return Mesh(data, name);
}

std::string Mesh::getName() const {
    return name;
}

glm::vec3 Mesh::getPosition() const {
    return glm::vec3(model[3]);
}

void Mesh::render(const Shader& shader, const glm::mat4& transform) const {
    shader.use(), shader.set<glm::mat4>("u_model", transform * model);
    buffer.bind(), glDrawArrays(GL_TRIANGLES, 0, (int)buffer.getSize());
}

void Mesh::setColor(const glm::vec3& color) {
    std::vector<Vertex> data = buffer.getData();
    std::for_each(data.begin(), data.end(), [color](Vertex& v) { v.color = color; });
    buffer = Buffer(data);
}

void Mesh::setModel(const glm::mat4& model) {
    this->model = model;
}
