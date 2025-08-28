#ifndef DEBUG_H
#define DEBUG_H

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "../Common/common.h"
#include "../Camera/camera.h"
#include "../Chunk/chunk.h"
#include "../World/world.h"
#include "../Shaders/shaders.h"

class Debugger {
    public:
    std::string camInfo = "Posição atual da camêra: [N/A]";
    std::string blockPosInfo = "Posição atual do bloco: [N/A]";
    std::string blockTypeInfo = "Tipo atual do bloco: [N/A]";
    std::string seedInfo = "Semente atual do mundo: [N/A]";
    std::string fpsInfo = "FPS: [Calculando...]";
    std::string chunkSolidInfo = "Quantidade de malhas (sólidas) da chunk atual: [Calculando...]";
    std::string chunkTransInfo = "Quantidade de malhas (translúcidas) da chunk atual: [Calculando...]";
    std::string chunkWaterInfo = "Quantidade de malhas (de água) da chunk atual: [Calculando...]";
    std::string chunkCrossInfo = "Quantidade de malhas (Cross-Shaped) da chunk atual: [Calculando...]";
    
    Debugger();
    ~Debugger();
    void updateBlockInfo(const World &world, const Vec3i pos);
    void updateCamInfo(const Camera &camera);
    void updateChunkInfo(const Chunk &chunk);
    void updateFPS();
    void updateSeed(const int seed);
    
    void drawChunkGrid(const Camera& camera, const glm::mat4& projection, const glm::mat4& view);
    
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
        int frameCount;
        unsigned int borderVAO, borderVBO;
        int borderVertexCount;
        unsigned int internalGridVAO, internalGridVBO;
        int internalGridVertexCount;
        int gridVertexCount;
        Shader gridShader;
        void check();
};

#endif // DEBUG_H