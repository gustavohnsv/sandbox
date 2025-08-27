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

        int getSolidCount() const;
        int getTransCount() const;
        int getWaterCount() const;
        int getCrossCount() const;

        void buildSolidMesh(const World &world, const Vec3i &chunkPos); // Destinado para sólidos. Ex: Grama, Terra, Pedra..
        void buildTransMesh(const World &world, const Vec3i &chunkPos); // Destinado para translúcidos. Ex: Folha, Vidro
        void buildWaterMesh(const World &world, const Vec3i &chunkPos); // Destinado para água
        void buildCrossMesh(const World &world, const Vec3i &chunkPos); // Destinado para sólidos "cross-shaped". Ex: Grama, Flor

        void drawSolid();
        void drawTrans();
        void drawWater();
        void drawCross();
        
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
        GLuint solid_VAO, solid_VBO;
        GLuint trans_VAO, trans_VBO;
        GLuint water_VAO, water_VBO;
        GLuint cross_VAO, cross_VBO;
        int solidCount, transCount, waterCount, crossCount;
        Vec3i pos;
        void check();
};

#endif // CHUNK_H