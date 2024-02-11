#pragma once

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

struct Atom {
    float radius, covalent;
    glm::vec3 color;
    double mass;
};

extern std::unordered_map<std::string, Atom> ptable;
extern std::unordered_map<int, std::string> an2sm;
extern std::unordered_map<std::string, int> sm2an;
