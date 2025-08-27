#include "structures.h"

#include "../World/world.h"

Structures::Structures() {}

void Structures::setBlockAcrossChunks(int type, World &world, const Vec3i &pos) {
    int chunkX = static_cast<int>(floor((float)pos.x / CHUNK_WIDTH));
    int chunkZ = static_cast<int>(floor((float)pos.z / CHUNK_DEPTH));
    
    Chunk* chunk = world.getChunk(chunkX, 0, chunkZ);
    if (chunk) {
        int localX = pos.x - chunkX * CHUNK_WIDTH;
        int localZ = pos.z - chunkZ * CHUNK_DEPTH;
        
        if (localX < 0) {
            localX += CHUNK_WIDTH;
            chunkX -= 1;
            chunk = world.getChunk(chunkX, 0, chunkZ);
        }
        if (localZ < 0) {
            localZ += CHUNK_DEPTH;
            chunkZ -= 1;
            chunk = world.getChunk(chunkX, 0, chunkZ);
        }
        if (localX >= CHUNK_WIDTH) {
            localX -= CHUNK_WIDTH;
            chunkX += 1;
            chunk = world.getChunk(chunkX, 0, chunkZ);
        }
        if (localZ >= CHUNK_DEPTH) {
            localZ -= CHUNK_DEPTH;
            chunkZ += 1;
            chunk = world.getChunk(chunkX, 0, chunkZ);
        }
        
        if (chunk && 
            localX >= 0 && localX < CHUNK_WIDTH && 
            localZ >= 0 && localZ < CHUNK_DEPTH &&
            pos.y >= 0 && pos.y < CHUNK_HEIGHT) {
            
            chunk->setBlock(localX, pos.y, localZ, type, false);
        }
    }
}

void Structures::placeStructure(int type, World &world, const Vec3i &pos) {
    int surfaceBlockType = world.getBlockType(pos);
    int offset = 0;
    switch (type) {
        case ID_ARVORE: // Árvore
            offset = 10;
            if (surfaceBlockType == ID_GRAMA) {

                int chunkX = static_cast<int>(floor((float)pos.x/CHUNK_WIDTH));
                int chunkZ = static_cast<int>(floor((float)pos.z/CHUNK_DEPTH));
                Vec3i chunkPos = { chunkX, 0, chunkZ };
                Chunk* chunk = world.getChunk(chunkPos.x, chunkPos.y, chunkPos.z);

                if (chunk) {
                    int localX = pos.x - chunkPos.x * CHUNK_WIDTH;
                    int localZ = pos.z - chunkPos.z * CHUNK_DEPTH;
                    for (int i = 0; i < 7; i++) {
                        if (i < 6) {
                            setBlockAcrossChunks(ID_CARVALHO, world, {pos.x, pos.y + i, pos.z});
                        }
                        if (i == 3 || i == 4) {
                            for (int k = -2; k < 3; k++) {
                                for (int j = -2; j < 3; j++) {
                                    if (k == 0 && j == 0) continue;
                                    setBlockAcrossChunks(ID_FOLHA_CARVALHO, world, {pos.x + k, pos.y + i, pos.z + j});
                                }
                            }
                        }
                        if (i == 5) {
                            for (int k = -1; k < 2; k++) {
                                for (int j = -1; j < 2; j++) {
                                    if (k == 0 && j == 0) continue;
                                    setBlockAcrossChunks(ID_FOLHA_CARVALHO, world, {pos.x + k, pos.y + i, pos.z + j});
                                }
                            }
                        }
                        if (i == 6) {
                        setBlockAcrossChunks(ID_FOLHA_CARVALHO, world, {pos.x + 1, pos.y + i, pos.z});
                        setBlockAcrossChunks(ID_FOLHA_CARVALHO, world, {pos.x - 1, pos.y + i, pos.z});
                        setBlockAcrossChunks(ID_FOLHA_CARVALHO, world, {pos.x, pos.y + i, pos.z + 1});
                        setBlockAcrossChunks(ID_FOLHA_CARVALHO, world, {pos.x, pos.y + i, pos.z - 1});
                        setBlockAcrossChunks(ID_FOLHA_CARVALHO, world, {pos.x, pos.y + i, pos.z});
                        }
                    }
                }
            } else if (surfaceBlockType == ID_GRAMA_NEVE) {
                int chunkX = static_cast<int>(floor((float)pos.x/CHUNK_WIDTH));
                int chunkZ = static_cast<int>(floor((float)pos.z/CHUNK_DEPTH));
                Vec3i chunkPos = { chunkX, 0, chunkZ };
                Chunk* chunk = world.getChunk(chunkPos.x, chunkPos.y, chunkPos.z);
                
                if (chunk) {
                    int localX = pos.x - chunkPos.x * CHUNK_WIDTH;
                    int localZ = pos.z - chunkPos.z * CHUNK_DEPTH;

                    for (int i = 0; i < 10; i++) {
                        chunk->setBlock(localX, pos.y + i, localZ, ID_PINHEIRO, false);
                    }
                }
            }
            break;
        case ID_FLOR:
            if (surfaceBlockType == ID_GRAMA && world.getBlockType(Vec3i{pos.x, pos.y+1, pos.z}) == ID_AR) {
                int oddFlower = rand() % 10;
                switch(oddFlower) {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                        setBlockAcrossChunks(ID_FLOR_MARGARIDA, world, {pos.x, pos.y+1, pos.z});
                        break;
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                        setBlockAcrossChunks(ID_FLOR_ROSA, world, {pos.x, pos.y+1, pos.z});
                        break;
                    deafult:
                        break;
                }
            }
            break;
        case ID_ARBUSTO:
            if (surfaceBlockType == ID_GRAMA && world.getBlockType(Vec3i{pos.x, pos.y+1, pos.z}) == ID_AR) {
                setBlockAcrossChunks(ID_GRAMA_BAIXA, world, {pos.x, pos.y+1, pos.z});
            }
            break;
        case ID_CACTUS:
            if (surfaceBlockType == ID_AREIA && world.getBlockType(Vec3i{pos.x, pos.y+1, pos.z}) == ID_AR) {
                for (int i = 1; i < 4; i++) {
                    setBlockAcrossChunks(ID_CACTO, world, {pos.x, pos.y+i, pos.z});
                }
            }
            break;
        default:
            break;
    }
}
        
void Structures::check() {
    return;
}