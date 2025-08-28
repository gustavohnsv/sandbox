#ifndef WORLD_H
#define WORLD_H

#include "../Common/common.h"

#include "../Chunk/chunk.h"
#include "../Shaders/shaders.h"
#include "../Other/FastNoiseLite.h"
#include "../Blocks/blocks.h"
#include "../Structures/structures.h"

class Shader;

class World {
    public:
        int seed;

        World(const Blocks &blocks, const Structures &structures);
        ~World();
        void create();
        Chunk* getChunk(int x, int y, int z);
        const Chunk* getChunk(int x, int y, int z) const;
        const std::unordered_map<Vec3i, Chunk, Vec3iHasher> getChunks() const;
        int getBlockType(const Vec3i &pos) const;
        std::string getBlockName(int type) const;
        Block getBlockInfo(int type) const;
        int getSeed() const;
        void addBlock(const Vec3i &pos, int type, bool isPlayerAction);
        void removeBlock(const Vec3i &pos);
        void update(const glm::vec3 &pos, const glm::mat4 &view, const glm::mat4 &proj);
        void generateChunkTerrain(Chunk &chunk, Vec3i chunkPos);
        void decorateChunk(Vec3i chunkPos);
        void draw(Shader &shader, const glm::mat4 &projection, const glm::mat4 &view, const glm::vec3 &cameraPos);
        void highlight(Shader &shader, const Vec3i &pos, const glm::mat4 &projection, const glm::mat4 &view);
        bool isExposedToSky(int x, int y, int z) const;
        bool hasBlockAt(const Vec3i &pos) const;
    
    private:
        std::filesystem::path saveDir;
        unsigned int VAO, VBO;
        unsigned int border_VAO, border_VBO;

        std::unordered_map<Vec3i, Chunk, Vec3iHasher> world;
        std::unordered_set<Vec3i, Vec3iHasher> chunksNeedingLighting;
        std::unordered_set<Vec3i, Vec3iHasher> chunksNeedingDecoration;

        Blocks blockDatabase;
        Structures structureDatabase;
        std::vector<Biome> biomes;

        FastNoiseLite noise;

        void requestChunk(int cx, int cz);
        bool allNeighborsExist(Vec3i chunkPos) const;
        void updateChunkLighting();
        void check();
};

#endif // WORLD_H