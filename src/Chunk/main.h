#ifndef CHUNK_H
#define CHUNK_H

#include "../Common/main.h"

const int CHUNK_WIDTH = 16;
const int CHUNK_HEIGHT = 128;
const int CHUNK_DEPTH = 16;

class Chunk {
    public:
        Chunk();
        bool saveFile(const std::string &filepath) const;
        bool loadFile(const std::string &filepath);
        int getCount() const;
        void buildMesh();
        void draw();
        void setBlock(int x, int y, int z, int type, bool isPlayerAction);
        void updateHeightMap();
        int getBlock(int x, int y, int z) const;
        bool isModified() const;

    private:
        int blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_DEPTH];
        int heightMap[CHUNK_WIDTH][CHUNK_DEPTH];
        bool modified;
        unsigned int VAO, VBO;
        int count;
        void check();
};

#endif // CHUNK_H