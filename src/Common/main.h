#ifndef COMMON_H
#define COMMON_H

// --- Bibliotecas Externas (Gráficos e Matemática) ---
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

// --- Bibliotecas Padrão do C++ ---
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

#endif // COMMON_H