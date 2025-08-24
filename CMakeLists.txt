cmake_minimum_required(VERSION 3.14)
project(sandbox)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

#FetchContent_Declare(
#  ftxui
#  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
#  GIT_TAG        v6.1.9
#)

FetchContent_Declare(
  GLFW
  GIT_REPOSITORY https://github.com/glfw/glfw
  GIT_TAG         3.4
)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        1.0.1
)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY  https://github.com/ocornut/imgui
  GIT_TAG         v1.90.8
)

FetchContent_MakeAvailable(glm GLFW)
#FetchContent_MakeAvailable(ftxui glm GLFW)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
  FetchContent_Populate(imgui)

  add_library(imgui
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
  )

  target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
  )
endif()

find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)

add_executable(
    sandbox 
    src/main.cpp 
    src/Shaders/shaders.cpp 
    src/World/world.cpp
    src/Camera/camera.cpp
    src/Chunk/chunk.cpp
    src/Texture/texture.cpp
    src/Player/player.cpp
    src/Debugger/debugger.cpp
    vendor/glad/src/glad.c
)

target_include_directories(sandbox PUBLIC vendor glad src ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)

target_link_libraries(sandbox PUBLIC glfw OpenGL X11::X11 glm imgui)
#target_link_libraries(sandbox PRIVATE ftxui::screen ftxui::component)