#include "structures.h"

#include "../World/world.h"

Structures::Structures() {
    // structures.insert(structures.begin() + ID_ARVORE, ID_ARVORE);
}

void Structures::placeStructure(int type, World &world, const Vec3i &pos) {
    int surfaceBlockType = world.getBlockType(pos);
    int offset = 0;
    switch (type) {
        case ID_ARVORE: // Árvore
            offset = 10;
            if (surfaceBlockType == ID_GRAMA) {
                for (int i = 0; i < 5; i++) {
                    world.addBlock(Vec3i{pos.x, pos.y+i, pos.z}, ID_CARVALHO, false);
                }
            } else if (surfaceBlockType == ID_GRAMA_NEVE) {
                for (int i = 0; i < 10; i++) {
                    world.addBlock(Vec3i{pos.x, pos.y+i, pos.z}, ID_PINHEIRO, false);
                }
            }
            break;
        default:
            break;
    }
    //int chunkX = static_cast<int>(floor((float)pos.x/CHUNK_WIDTH));
    //int chunkZ = static_cast<int>(floor((float)pos.z/CHUNK_HEIGHT));
    //Chunk* chunk = world.getChunk(pos.x, pos.y, pos.z);
    //int surfaceY = chunk->getHeightValue(chunkX, chunkZ);
    //int surfaceBlockType = chunk->getBlock(chunkX, surfaceY, chunkZ);
    //int offset = 0;
    //switch (type) {
    //    case ID_ARVORE: // Árvore
    //        offset = 10;
    //        for (int x = pos.x-offset; x < pos.x+offset; x++) {
    //            for (int z = pos.z-offset; z < pos.z+offset; z++) {
    //                for (int y = surfaceY-offset; y < surfaceY+offset; y++) {
    //                    if (chunk->getBlock(chunkX, y, chunkZ) == ID_CARVALHO || chunk->getBlock(chunkX, y, chunkZ) == ID_PINHEIRO) return;
    //                }
    //            }
    //        }
    //        if (surfaceBlockType == ID_GRAMA) {
    //            for (int i = 0; i < 5; i++) {
    //                chunk->setBlock(pos.x, surfaceY+i, pos.z, ID_CARVALHO, false);
    //            }
    //            chunk->setBlock(pos.x, surfaceY, pos.z, ID_TERRA, false);
    //        } else if (surfaceBlockType == ID_GRAMA_NEVE) {
    //            for (int i = 0; i < 10; i++) {
    //                
    //                chunk->setBlock(pos.x, surfaceY+i, pos.z, ID_PINHEIRO, false);
    //            }
    //            chunk->setBlock(pos.x, surfaceY, pos.z, ID_TERRA, false);
    //        }
    //        break;
    //    default:
    //        std::cout << "Não foi possível adicionar nenhuma estrutura!";
    //        break;
    //}
}
        
void Structures::check() {
    return;
}