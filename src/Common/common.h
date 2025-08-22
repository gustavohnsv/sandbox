#ifndef COMMON_H
#define COMMON_H

// --- Bibliotecas Externas (Gráficos e Matemática) ---
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// --- Bibliotecas Padrão do C++ ---
#include <string_view>
#include <filesystem>
#include <optional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include <map>
#include <set>

// --- Bibliotecas Padrão do C ---
#include <ctime> // Para a função time()

struct Vec3i {
    int x, y, z;

    // Sobrecarga do operador '<' para permitir o uso de Vec3i como chave em um std::map
    bool operator<(const Vec3i &other) const {
        if (x != other.x) return x < other.x;
        if (y != other.y) return y < other.y;
        return z < other.z;
    }
};

struct Vec2i {
    int x, z;
    
    // Sobrecarga do operador '<'
    bool operator<(const Vec2i &other) const {
        if (x != other.x) return x < other.x;
        return z < other.z;
    }
};

struct Block {
    const char* name;
    int type;

    bool operator<(const Block &other) const {
        return type < other.type;
    }
};

enum FrustumPlane {
    F_LEFT = 0,
    F_RIGHT = 1,
    F_BOTTOM = 2,
    F_TOP = 3,
    F_NEAR = 4,
    F_FAR = 5
};

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

struct Frustum {
    glm::vec4 planes[6];
    void update(const glm::mat4 &view, const glm::mat4 &proj) {
        const glm::mat4 viewProj = proj * view;
        glm::vec4 rows[4] = {
            glm::row(viewProj, 0),
            glm::row(viewProj, 1),
            glm::row(viewProj, 2),
            glm::row(viewProj, 3)
        };
        planes[F_LEFT] = rows[3] + rows[0];
        planes[F_RIGHT] = rows[3] - rows[0];
        planes[F_BOTTOM] = rows[3] + rows[1];
        planes[F_TOP] = rows[3] - rows[1];
        planes[F_NEAR] = rows[3] - rows[2];
        planes[F_FAR] = rows[3] + rows[2];
        for (int i = 0; i < 6; i++) {
            planes[i] = planes[i]/glm::length(glm::vec3(planes[i]));
        }
    }
    bool isBoxInFrustum(const AABB &box) {
        for (int i = 0; i < 6; i++) {
            glm::vec3 p_vertex;
            if (planes[i].x > 0) {
                p_vertex.x = box.max.x;
            } else {
                p_vertex.x = box.min.x;
            }
            if (planes[i].y > 0) {
                p_vertex.y = box.max.y;
            } else {
                p_vertex.y = box.min.y;
            }
            if (planes[i].z > 0) {
                p_vertex.z = box.max.z;
            } else {
                p_vertex.z = box.min.z;
            }

            float dist = glm::dot(glm::vec3(planes[i]), p_vertex) + planes[i].w;
            if (dist < 0) return false;
        }
        return true;
    }
};

#endif // COMMON_H