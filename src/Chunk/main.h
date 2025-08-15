#ifndef CHUNK_H
#define CHUNK_H

#include "../Common/main.h"

const int CHUNK_WIDTH = 16;
const int CHUNK_HEIGHT = 64;
const int CHUNK_DEPTH = 16;

class Chunk {
    public:
        Chunk();
        void buildMesh();
        void draw();
        void setBlock(int x, int y, int z, int type);
        int getBlock(int x, int y, int z) const;

    private:
        int blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_DEPTH];
        unsigned int VAO, VBO;
        int count;
        void check();
};

#endif // CHUNK_H