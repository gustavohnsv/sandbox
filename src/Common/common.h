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
#include <unordered_map>
#include <unordered_set>
#include <string_view>
#include <filesystem>
#include <algorithm>
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

const int ID_AR             = 0;
const int ID_GRAMA          = 1;
const int ID_TERRA          = 2;
const int ID_PEDRA          = 3;
const int ID_ROCHA_MATRIZ   = 4;
const int ID_AGUA           = 5;
const int ID_AREIA          = 6;
const int ID_ARENITO        = 7;
const int ID_GRAMA_NEVE     = 8;
const int ID_ABOBORA        = 99; // Vai mudar depois

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

    bool operator==(const Vec3i &other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

struct Vec3iHasher {
    std::size_t operator()(const Vec3i &k) const {
        std::size_t h1 = std::hash<int>()(k.x);
        std::size_t h2 = std::hash<int>()(k.y);
        std::size_t h3 = std::hash<int>()(k.z);
        return h1 ^ (h2 << 1) ^ h3;
    }
};

struct Vec2i {
    int x, y;
    
    // Sobrecarga do operador '<'
    bool operator<(const Vec2i &other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }

    void operator=(const Vec2i &other) {
        x = other.x;
        y = other.y;
        return;
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
        planes[F_LEFT]      = rows[3] + rows[0];
        planes[F_RIGHT]     = rows[3] - rows[0];
        planes[F_BOTTOM]    = rows[3] + rows[1];
        planes[F_TOP]       = rows[3] - rows[1];
        planes[F_NEAR]      = rows[3] - rows[2];
        planes[F_FAR]       = rows[3] + rows[2];
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


struct Profiler {
    template<typename Func>
    static void measure(const std::string& name, Func function_to_run) {
        auto start = std::chrono::high_resolution_clock::now();
        function_to_run();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Tempo gasto em " << name << ": " << duration.count() << "ms" << std::endl;
    }
};

struct Biome {
    std::string name;
    int topBlock;
    int subsurfaceBlock;

    float baseHeight;
    float amplitude;
    float frequency;

    float idealTemp;
    float idealHumidity;

    bool allowLakes;
};

struct Block {
    std::string name;
    int type;
    
    Vec2i tex_top;
    Vec2i tex_side;
    Vec2i tex_bottom;
};

#endif // COMMON_H