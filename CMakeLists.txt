cmake_minimum_required(VERSION 3.5)
project(sandbox)
set(CMAKE_CXX_STANDARD 17)

# Adicionamos o GLFW como um subdiretório
add_subdirectory(external/glfw-3.4)

find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)

add_executable(
    sandbox 
    src/main.cpp 
    src/glad.c 
    src/Shaders/main.cpp 
    src/World/main.cpp
    src/Camera/main.cpp
    src/Chunk/main.cpp
    src/Texture/main.cpp
)

target_include_directories(sandbox PUBLIC include)

target_link_libraries(sandbox PUBLIC glfw OpenGL X11::X11)