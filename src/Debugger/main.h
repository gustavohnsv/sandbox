#ifndef DEBUG_H
#define DEBUG_H

#include "../Common/main.h"
#include "../Camera/main.h"
#include "../Chunk/main.h"
#include "../World/main.h"
#include "../Shaders/main.h"

class Debugger {
    public:
    std::string camInfo = "Camêra: [N/A]";
    std::string blockPosInfo = "Bloco: [N/A]";
    std::string blockTypeInfo = "Tipo: [N/A]";
    std::string seedInfo = "Semente: [N/A]";
    std::string fpsInfo = "FPS: [Calculando...]";
    std::string chunkInfo = "Malha: [Calculando...]";
    
    Debugger();
    void updateBlockInfo(const World &world, const Vec3i pos);
    void updateCamInfo(const Camera &camera);
    void updateChunkInfo(const Chunk &chunk);
    void updateFPS();
    void updateSeed(const int seed);

    void drawChunkGrid(const Camera& camera, const glm::mat4& projection, const glm::mat4& view);

    ftxui::Element Render();
    
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