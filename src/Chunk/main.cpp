#include "main.h"

#include "../World/main.h"

Chunk::Chunk() {
    memset(blocks, 0, sizeof(blocks));
    memset(heightMap, -1, sizeof(heightMap));
    VAO = 0;
    VBO = 0;
    water_VAO = 0;
    water_VBO = 0;
    count = 0;
    waterCount = 0;
    modified = false;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    int stride = 10 * sizeof(float);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);

    glGenVertexArrays(1, &water_VAO);
    glGenBuffers(1, &water_VBO);

    glBindVertexArray(water_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, water_VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);

}

bool Chunk::saveFile(const std::string &filepath) const {
    std::ofstream output(filepath, std::ios::binary);
    if (!output) {
        std::cout << "Erro ao gerar arquivo de salvamento" << std::endl;
        return false;
    }
    output.write(reinterpret_cast<const char*>(blocks), sizeof(blocks));
    output.close();
    return true;
}

bool Chunk::loadFile(const std::string &filepath, const World &world, const Vec3i &chunkPos) {
    std::ifstream input(filepath, std::ios::binary);
    if (!input) {
        return false;
    }
    input.read(reinterpret_cast<char*>(blocks), sizeof(blocks));
    input.close();
    updateHeightMap();
    buildMesh(world, chunkPos);
    buildWaterMesh(world, chunkPos);
    return true;
}

int Chunk::getCount() const {
    return count;
}

void Chunk::buildMesh(const World &world, const Vec3i &chunkPos) {
    std::vector<float> mesh;
    float baseLight = 0.1;
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_DEPTH; z++) {
                int type = blocks[x][y][z];
                if (type == 0 || type == 5) continue;
                int globalX = chunkPos.x * CHUNK_WIDTH + x;
                int globalY = chunkPos.y * CHUNK_HEIGHT + y;
                int globalZ = chunkPos.z * CHUNK_DEPTH + z;
                float skylight_top = world.isExposedToSky(globalX, globalY, globalZ) ? 0.5: baseLight;
                float skylight_other = baseLight;
                int blockAbove = world.getBlockType(Vec3i{globalX, globalY + 1, globalZ});
                if (blockAbove == 0 || blockAbove == 5) {
                    mesh.insert(mesh.end(), {
                        x - 0.5f, y + 0.5f, z - 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,        0.0f, 1.0f,     skylight_top,
                        x + 0.5f, y + 0.5f, z - 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,        1.0f, 1.0f,     skylight_top,
                        x + 0.5f, y + 0.5f, z + 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,        1.0f, 0.0f,     skylight_top,
                        x + 0.5f, y + 0.5f, z + 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,        1.0f, 0.0f,     skylight_top,
                        x - 0.5f, y + 0.5f, z + 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,        0.0f, 0.0f,     skylight_top,
                        x - 0.5f, y + 0.5f, z - 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,        0.0f, 1.0f,     skylight_top,
                    });
                }
                int blockBelow = world.getBlockType(Vec3i{globalX, globalY - 1, globalZ});
                if (blockBelow == 0 || blockBelow == 5) {
                    mesh.insert(mesh.end(), {
                        x - 0.5f, y - 0.5f, z - 0.5f,   0.0f,-1.0f, 0.0f,   (float)type,        0.0f, 1.0f,     skylight_other,
                        x + 0.5f, y - 0.5f, z + 0.5f,   0.0f,-1.0f, 0.0f,   (float)type,        1.0f, 1.0f,     skylight_other,
                        x + 0.5f, y - 0.5f, z - 0.5f,   0.0f,-1.0f, 0.0f,   (float)type,        1.0f, 0.0f,     skylight_other,
                        x + 0.5f, y - 0.5f, z + 0.5f,   0.0f,-1.0f, 0.0f,   (float)type,        1.0f, 0.0f,     skylight_other,
                        x - 0.5f, y - 0.5f, z - 0.5f,   0.0f,-1.0f, 0.0f,   (float)type,        0.0f, 0.0f,     skylight_other,
                        x - 0.5f, y - 0.5f, z + 0.5f,   0.0f,-1.0f, 0.0f,   (float)type,        0.0f, 1.0f,     skylight_other,
                    });
                }
                
                // FACE DIREITA (+X) - verificar bloco à direita
                int blockRight = world.getBlockType(Vec3i{globalX + 1, globalY, globalZ});
                if (blockRight == 0 || blockRight == 5) {
                    mesh.insert(mesh.end(), {
                        x + 0.5f, y - 0.5f, z - 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,        0.0f, 1.0f,     skylight_other,
                        x + 0.5f, y - 0.5f, z + 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,        1.0f, 1.0f,     skylight_other,
                        x + 0.5f, y + 0.5f, z + 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,        1.0f, 0.0f,     skylight_other,
                        x + 0.5f, y + 0.5f, z + 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,        1.0f, 0.0f,     skylight_other,
                        x + 0.5f, y + 0.5f, z - 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,        0.0f, 0.0f,     skylight_other,
                        x + 0.5f, y - 0.5f, z - 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,        0.0f, 1.0f,     skylight_other,
                    });
                }
                
                // FACE ESQUERDA (-X) - verificar bloco à esquerda
                int blockLeft = world.getBlockType(Vec3i{globalX - 1, globalY, globalZ});
                if (blockLeft == 0 || blockLeft == 5) {
                    mesh.insert(mesh.end(), {
                        x - 0.5f, y - 0.5f, z - 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,        0.0f, 1.0f,     skylight_other,
                        x - 0.5f, y + 0.5f, z + 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,        1.0f, 1.0f,     skylight_other,
                        x - 0.5f, y - 0.5f, z + 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,        1.0f, 0.0f,     skylight_other,
                        x - 0.5f, y + 0.5f, z + 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,        1.0f, 0.0f,     skylight_other,
                        x - 0.5f, y - 0.5f, z - 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,        0.0f, 0.0f,     skylight_other,
                        x - 0.5f, y + 0.5f, z - 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,        0.0f, 1.0f,     skylight_other,
                    });
                }
                
                // FACE FRENTE (+Z) - verificar bloco na frente
                int blockFront = world.getBlockType(Vec3i{globalX, globalY, globalZ + 1});
                if (blockFront == 0 || blockFront == 5) {
                    mesh.insert(mesh.end(), {
                        x - 0.5f, y - 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,        0.0f, 1.0f,     skylight_other,
                        x + 0.5f, y - 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,        1.0f, 1.0f,     skylight_other,
                        x + 0.5f, y + 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,        1.0f, 0.0f,     skylight_other,
                        x + 0.5f, y + 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,        1.0f, 0.0f,     skylight_other,
                        x - 0.5f, y + 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,        0.0f, 0.0f,     skylight_other,
                        x - 0.5f, y - 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,        0.0f, 1.0f,     skylight_other,
                    });
                }
                
                // FACE TRÁS (-Z) - verificar bloco atrás
                int blockBack = world.getBlockType(Vec3i{globalX, globalY, globalZ - 1});
                if (blockBack == 0 || blockBack == 5) {
                    mesh.insert(mesh.end(), {
                        x - 0.5f, y - 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   (float)type,        0.0f, 1.0f,     skylight_other,
                        x + 0.5f, y + 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   (float)type,        1.0f, 1.0f,     skylight_other,
                        x + 0.5f, y - 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   (float)type,        1.0f, 0.0f,     skylight_other,
                        x + 0.5f, y + 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   (float)type,        1.0f, 0.0f,     skylight_other,
                        x - 0.5f, y - 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   (float)type,        0.0f, 0.0f,     skylight_other,
                        x - 0.5f, y + 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   (float)type,        0.0f, 1.0f,     skylight_other,
                    });
                }
            }
        }
    }
    if (!mesh.empty()) {
        count = mesh.size() / 10;
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(float), mesh.data(), GL_STATIC_DRAW);
    } else {
        count = 0;
    }
}

void Chunk::buildWaterMesh(const World &world, const Vec3i &chunkPos) {
    std::vector<float> mesh;
    float baseLight = 0.1;
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_DEPTH; z++) {
                if (blocks[x][y][z] != 5) continue;
                int type = 5;
                int globalX = chunkPos.x * CHUNK_WIDTH + x;
                int globalY = chunkPos.y * CHUNK_HEIGHT + y;
                int globalZ = chunkPos.z * CHUNK_DEPTH + z;
                float skylight_top = world.isExposedToSky(globalX, globalY, globalZ) ? 0.5: baseLight;
                float skylight_other = baseLight;
                
                // FACE SUPERIOR - verificar bloco acima (pode estar em outro chunk)
                int blockAbove = world.getBlockType(Vec3i{globalX, globalY + 1, globalZ});
                if (blockAbove == 0) { // Só desenhar se há ar acima
                    mesh.insert(mesh.end(), {
                        x - 0.5f, y + 0.4f, z - 0.5f,   0.0f, 1.0f, 0.0f,   5.0f,        0.0f, 1.0f,     skylight_top,
                        x + 0.5f, y + 0.4f, z - 0.5f,   0.0f, 1.0f, 0.0f,   5.0f,        1.0f, 1.0f,     skylight_top,
                        x + 0.5f, y + 0.4f, z + 0.5f,   0.0f, 1.0f, 0.0f,   5.0f,        1.0f, 0.0f,     skylight_top,
                        x + 0.5f, y + 0.4f, z + 0.5f,   0.0f, 1.0f, 0.0f,   5.0f,        1.0f, 0.0f,     skylight_top,
                        x - 0.5f, y + 0.4f, z + 0.5f,   0.0f, 1.0f, 0.0f,   5.0f,        0.0f, 0.0f,     skylight_top,
                        x - 0.5f, y + 0.4f, z - 0.5f,   0.0f, 1.0f, 0.0f,   5.0f,        0.0f, 1.0f,     skylight_top,
                    });
                }
                
                // FACE INFERIOR - verificar bloco abaixo
                int blockBelow = world.getBlockType(Vec3i{globalX, globalY - 1, globalZ});
                if (blockBelow == 0) {
                    mesh.insert(mesh.end(), {
                        x - 0.5f, y - 0.5f, z - 0.5f,   0.0f,-1.0f, 0.0f,   5.0f,        0.0f, 1.0f,     skylight_other,
                        x + 0.5f, y - 0.5f, z + 0.5f,   0.0f,-1.0f, 0.0f,   5.0f,        1.0f, 1.0f,     skylight_other,
                        x + 0.5f, y - 0.5f, z - 0.5f,   0.0f,-1.0f, 0.0f,   5.0f,        1.0f, 0.0f,     skylight_other,
                        x + 0.5f, y - 0.5f, z + 0.5f,   0.0f,-1.0f, 0.0f,   5.0f,        1.0f, 0.0f,     skylight_other,
                        x - 0.5f, y - 0.5f, z - 0.5f,   0.0f,-1.0f, 0.0f,   5.0f,        0.0f, 0.0f,     skylight_other,
                        x - 0.5f, y - 0.5f, z + 0.5f,   0.0f,-1.0f, 0.0f,   5.0f,        0.0f, 1.0f,     skylight_other,
                    });
                }
                
                // FACE DIREITA (+X) - verificar bloco à direita
                int blockRight = world.getBlockType(Vec3i{globalX + 1, globalY, globalZ});
                if (blockRight == 0) {
                    mesh.insert(mesh.end(), {
                        x + 0.5f, y - 0.5f, z - 0.5f,   1.0f, 0.0f, 0.0f,   5.0f,        0.0f, 1.0f,     skylight_other,
                        x + 0.5f, y - 0.5f, z + 0.5f,   1.0f, 0.0f, 0.0f,   5.0f,        1.0f, 1.0f,     skylight_other,
                        x + 0.5f, y + 0.5f, z + 0.5f,   1.0f, 0.0f, 0.0f,   5.0f,        1.0f, 0.0f,     skylight_other,
                        x + 0.5f, y + 0.5f, z + 0.5f,   1.0f, 0.0f, 0.0f,   5.0f,        1.0f, 0.0f,     skylight_other,
                        x + 0.5f, y + 0.5f, z - 0.5f,   1.0f, 0.0f, 0.0f,   5.0f,        0.0f, 0.0f,     skylight_other,
                        x + 0.5f, y - 0.5f, z - 0.5f,   1.0f, 0.0f, 0.0f,   5.0f,        0.0f, 1.0f,     skylight_other,
                    });
                }
                
                // FACE ESQUERDA (-X) - verificar bloco à esquerda
                int blockLeft = world.getBlockType(Vec3i{globalX - 1, globalY, globalZ});
                if (blockLeft == 0) {
                    mesh.insert(mesh.end(), {
                        x - 0.5f, y - 0.5f, z - 0.5f,  -1.0f, 0.0f, 0.0f,   5.0f,        0.0f, 1.0f,     skylight_other,
                        x - 0.5f, y + 0.5f, z + 0.5f,  -1.0f, 0.0f, 0.0f,   5.0f,        1.0f, 1.0f,     skylight_other,
                        x - 0.5f, y - 0.5f, z + 0.5f,  -1.0f, 0.0f, 0.0f,   5.0f,        1.0f, 0.0f,     skylight_other,
                        x - 0.5f, y + 0.5f, z + 0.5f,  -1.0f, 0.0f, 0.0f,   5.0f,        1.0f, 0.0f,     skylight_other,
                        x - 0.5f, y - 0.5f, z - 0.5f,  -1.0f, 0.0f, 0.0f,   5.0f,        0.0f, 0.0f,     skylight_other,
                        x - 0.5f, y + 0.5f, z - 0.5f,  -1.0f, 0.0f, 0.0f,   5.0f,        0.0f, 1.0f,     skylight_other,
                    });
                }
                
                // FACE FRENTE (+Z) - verificar bloco na frente
                int blockFront = world.getBlockType(Vec3i{globalX, globalY, globalZ + 1});
                if (blockFront == 0) {
                    mesh.insert(mesh.end(), {
                        x - 0.5f, y - 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   5.0f,        0.0f, 1.0f,     skylight_other,
                        x + 0.5f, y - 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   5.0f,        1.0f, 1.0f,     skylight_other,
                        x + 0.5f, y + 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   5.0f,        1.0f, 0.0f,     skylight_other,
                        x + 0.5f, y + 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   5.0f,        1.0f, 0.0f,     skylight_other,
                        x - 0.5f, y + 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   5.0f,        0.0f, 0.0f,     skylight_other,
                        x - 0.5f, y - 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   5.0f,        0.0f, 1.0f,     skylight_other,
                    });
                }
                
                // FACE TRÁS (-Z) - verificar bloco atrás
                int blockBack = world.getBlockType(Vec3i{globalX, globalY, globalZ - 1});
                if (blockBack == 0) {
                    mesh.insert(mesh.end(), {
                        x - 0.5f, y - 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   5.0f,        0.0f, 1.0f,     skylight_other,
                        x + 0.5f, y + 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   5.0f,        1.0f, 1.0f,     skylight_other,
                        x + 0.5f, y - 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   5.0f,        1.0f, 0.0f,     skylight_other,
                        x + 0.5f, y + 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   5.0f,        1.0f, 0.0f,     skylight_other,
                        x - 0.5f, y - 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   5.0f,        0.0f, 0.0f,     skylight_other,
                        x - 0.5f, y + 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   5.0f,        0.0f, 1.0f,     skylight_other,
                    });
                }
            }
        }
    }
    if (!mesh.empty()) {
        waterCount = mesh.size() / 10;
        glBindBuffer(GL_ARRAY_BUFFER, water_VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(float), mesh.data(), GL_STATIC_DRAW);
    } else {
        waterCount = 0;
    }
}

void Chunk::draw() {
    if (count == 0) return;
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, count);
    glBindVertexArray(0);
}

void Chunk::drawWater() {
    if (waterCount == 0) return;
    glBindVertexArray(water_VAO);
    glDrawArrays(GL_TRIANGLES, 0, waterCount);
    glBindVertexArray(0);
}

void Chunk::setBlock(int x, int y, int z, int type, bool isPlayerAction) {
    if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_HEIGHT && z >= 0 && z < CHUNK_DEPTH) {
        blocks[x][y][z] = type;
        if (isPlayerAction) modified = true;
    }
}

void Chunk::updateHeightMap() {
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int z = 0; z < CHUNK_DEPTH; z++) {
            heightMap[x][z] = -1;
            for (int y = CHUNK_HEIGHT-1; y >= 0; y--) {
                if (blocks[x][y][z] != 0) {
                    heightMap[x][z] = y;
                    break;
                }
            }
        }
    }
}

int Chunk::getHeightValue(int x, int z) const {
    if (x >= 0 && x < CHUNK_WIDTH && z >= 0 && z < CHUNK_DEPTH) {
        return heightMap[x][z];
    }
    return 0;
}

int Chunk::getBlock(int x, int y, int z) const {
    if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_HEIGHT && z >= 0 && z < CHUNK_DEPTH) {
        return blocks[x][y][z];
    }
    return 0;
}

bool Chunk::isModified() const {
    return modified;
}

void Chunk::check() {
    return;
}