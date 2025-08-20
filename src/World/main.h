#ifndef WORLD_H
#define WORLD_H

#include "../Common/main.h"

#include "../Chunk/main.h"
#include "../Common/main.h"
#include "../Shaders/main.h"
#include "../Other/FastNoiseLite.h"

const int ID_AR             = 0;
const int ID_GRAMA          = 1;
const int ID_TERRA          = 2;
const int ID_PEDRA          = 3;
const int ID_ROCHA_MATRIZ   = 4;
const int ID_AGUA           = 5;

class Shader;

class World {
    public:
        int seed;

        World();
        void create();
        Chunk* getChunk(int x, int y, int z);
        const Chunk* getChunk(int x, int y, int z) const;
        const std::map<Vec3i, Chunk> getChunks() const;
        int getBlockType(const Vec3i &pos) const;
        int getSeed() const;
        void addBlock(const Vec3i &pos);
        void removeBlock(const Vec3i &pos);
        void update(const glm::vec3 &pos);
        void generateChunkData(Chunk &chunk, Vec3i Chunkpos);
        void draw(Shader &shader, const glm::mat4 &projection, const glm::mat4 &view, const glm::vec3 &cameraPos);
        void highlight(Shader &shader, const Vec3i &pos, const glm::mat4 &projection, const glm::mat4 &view);
        bool isExposedToSky(int x, int y, int z) const;
        bool hasBlockAt(const Vec3i &pos) const;
    
    private:
        std::filesystem::path saveDir;
        unsigned int VAO, VBO;
        unsigned int border_VAO, border_VBO;
        std::map<Vec3i, Chunk> world;
        std::set<Vec3i> chunksNeedingLighting;
        FastNoiseLite noise;
        void updateChunkLighting();
        void check();
};

#endif // WORLD_H