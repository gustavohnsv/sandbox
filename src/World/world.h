#ifndef WORLD_H
#define WORLD_H

#include "../Common/common.h"

#include "../Chunk/chunk.h"
#include "../Shaders/shaders.h"
#include "../Other/FastNoiseLite.h"

class Shader;

class World {
    public:
        int seed;

        World();
        void create();
        Chunk* getChunk(int x, int y, int z);
        const Chunk* getChunk(int x, int y, int z) const;
        const std::unordered_map<Vec3i, Chunk, Vec3iHasher> getChunks() const;
        int getBlockType(const Vec3i &pos) const;
        std::string getBlockName(int type) const;
        Block getBlockInfo(int type) const;
        int getSeed() const;
        void addBlock(const Vec3i &pos);
        void removeBlock(const Vec3i &pos);
        void update(const glm::vec3 &pos, const glm::mat4 &view, const glm::mat4 &proj);
        void generateChunkData(Chunk &chunk, Vec3i Chunkpos);
        void draw(Shader &shader, const glm::mat4 &projection, const glm::mat4 &view, const glm::vec3 &cameraPos);
        void highlight(Shader &shader, const Vec3i &pos, const glm::mat4 &projection, const glm::mat4 &view);
        bool isExposedToSky(int x, int y, int z) const;
        bool hasBlockAt(const Vec3i &pos) const;
    
    private:
        std::map<int, std::string> blockSummary;
        std::filesystem::path saveDir;
        unsigned int VAO, VBO;
        unsigned int border_VAO, border_VBO;

        std::unordered_map<Vec3i, Chunk, Vec3iHasher> world;
        std::unordered_set<Vec3i, Vec3iHasher> chunksNeedingLighting;

        FastNoiseLite noise;
        void updateChunkLighting();
        void check();
};

#endif // WORLD_H