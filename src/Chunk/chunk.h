#ifndef CHUNK_H
#define CHUNK_H

#include "../Common/common.h"

const int CHUNK_WIDTH = 16;
const int CHUNK_HEIGHT = 128;
const int CHUNK_DEPTH = 16;

class World;

class Chunk {
    public:
        Chunk();
        bool saveFile(const std::string &filepath) const;
        bool loadFile(const std::string &filepath, const World &world, const Vec3i &chunkPos);
        int getCount() const;
        void buildMesh(const World &world, const Vec3i &chunkPos);
        void buildWaterMesh(const World &world, const Vec3i &chunkPos);
        void draw();
        void drawWater();
        void setBlock(int x, int y, int z, int type, bool isPlayerAction);
        void updateHeightMap();
        void setPosition(const Vec3i& pos);
        Vec3i getPosition() const;
        int getHeightValue(int x, int z) const;
        int getBlock(int x, int y, int z) const;
        bool isModified() const;

    private:
        int blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_DEPTH];
        int heightMap[CHUNK_WIDTH][CHUNK_DEPTH];
        bool modified;
        unsigned int VAO, VBO;
        unsigned int water_VAO, water_VBO;
        int count;
        int waterCount;
        Vec3i pos;
        void check();
};

#endif // CHUNK_H