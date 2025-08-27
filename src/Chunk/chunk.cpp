#include "chunk.h"

#include "../World/world.h"

Chunk::Chunk() {
    memset(blocks, 0, sizeof(blocks));
    memset(heightMap, -1, sizeof(heightMap));
    solid_VAO = 0;
    solid_VBO = 0;
    trans_VAO = 0;
    trans_VBO = 0;
    water_VAO = 0;
    water_VBO = 0;
    cross_VAO = 0;
    cross_VBO = 0;
    solidCount = 0;
    transCount = 0;
    waterCount = 0;
    crossCount = 0;
    modified = false;
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
    auto start_file = std::chrono::high_resolution_clock::now();
    std::ifstream input(filepath, std::ios::binary);
    if (!input) {
        return false;
    }
    input.read(reinterpret_cast<char*>(blocks), sizeof(blocks));
    input.close();
    updateHeightMap();
    return true;
}

int Chunk::getSolidCount() const {
    return solidCount;
}

int Chunk::getTransCount() const {
    return transCount;
}

int Chunk::getWaterCount() const {
    return waterCount;
}

int Chunk::getCrossCount() const {
    return crossCount;
}

void Chunk::buildSolidMesh(const World &world, const Vec3i &chunkPos) {
    if (solid_VAO == 0) {
        glGenVertexArrays(1, &solid_VAO);
        glGenBuffers(1, &solid_VBO);

        glBindVertexArray(solid_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, solid_VBO);

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
    }
    std::vector<float> mesh;
    float baseLight = 0.5f;
    float offset = 0.5f;
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_DEPTH; z++) {
                int type = blocks[x][y][z];
                if (getBlockRenderType(type) != SOLID) continue;
                int globalX = chunkPos.x * CHUNK_WIDTH + x;
                int globalY = chunkPos.y * CHUNK_HEIGHT + y;
                int globalZ = chunkPos.z * CHUNK_DEPTH + z;
                const Block &blockInfo = world.getBlockInfo(type);
                float atlasW = 1024.0f;
                float atlasH = 512.0f;
                float tileW = 16.0f;
                float tileH = 16.0f;
                float uv_x_step = tileW / atlasW;
                float uv_y_step = tileH / atlasH;

                // FACE CIMA (+Y) - verificar bloco acima
                int blockAbove = world.getBlockType(Vec3i{globalX, globalY + 1, globalZ});
                if (blockAbove == -1) blockAbove = ID_AR;
                if (blockAbove == ID_AR || 
                    blockAbove == ID_AGUA || 
                    getBlockRenderType(blockAbove) == TRANSLUCENT || 
                    getBlockRenderType(blockAbove) == CROSS) {
                        float u_min = blockInfo.tex_top.x * uv_x_step;
                        float v_min = blockInfo.tex_top.y * uv_y_step;
                        float u_max = u_min + uv_x_step;
                        float v_max = v_min + uv_y_step;
                        float faceLight = world.isExposedToSky(globalX, globalY + 1, globalZ) ? 1.0 : baseLight;
                        if (type == ID_PEDRA) faceLight -= 0.4; 
                        mesh.insert(mesh.end(), {
                            // Triângulo 1
                            x - offset, y + offset, z - offset,   0.0f, 1.0f, 0.0f,   (float)type,   u_min, v_min,   faceLight,
                            x + offset, y + offset, z - offset,   0.0f, 1.0f, 0.0f,   (float)type,   u_max, v_min,   faceLight,
                            x + offset, y + offset, z + offset,   0.0f, 1.0f, 0.0f,   (float)type,   u_max, v_max,   faceLight,
                            // Triângulo 2
                            x + offset, y + offset, z + offset,   0.0f, 1.0f, 0.0f,   (float)type,   u_max, v_max,   faceLight,
                            x - offset, y + offset, z + offset,   0.0f, 1.0f, 0.0f,   (float)type,   u_min, v_max,   faceLight,
                            x - offset, y + offset, z - offset,   0.0f, 1.0f, 0.0f,   (float)type,   u_min, v_min,   faceLight,
                            });
                }

                // FACE BAIXO (-Y) - verificar bloco debaixo
                int blockBelow = world.getBlockType(Vec3i{globalX, globalY - 1, globalZ});
                if (blockBelow == -1) blockBelow = ID_AR;
                if (blockBelow == ID_AR || 
                    blockBelow == ID_AGUA || 
                    getBlockRenderType(blockBelow) == TRANSLUCENT ||
                    getBlockRenderType(blockBelow) == CROSS) {
                        float u_min = blockInfo.tex_bottom.x * uv_x_step;
                        float v_min = blockInfo.tex_bottom.y * uv_y_step;
                        float u_max = u_min + uv_x_step;
                        float v_max = v_min + uv_y_step;
                        float faceLight = world.isExposedToSky(globalX, globalY - 1, globalZ) ? 0.7 : baseLight;
                        if (type == ID_PEDRA) faceLight -= 0.4;
                        mesh.insert(mesh.end(), {
                            // Triângulo 1
                            x - offset, y - offset, z - offset,   0.0f, -1.0f, 0.0f,   (float)type,   u_min, v_max,   faceLight,
                            x + offset, y - offset, z + offset,   0.0f, -1.0f, 0.0f,   (float)type,   u_max, v_min,   faceLight,
                            x + offset, y - offset, z - offset,   0.0f, -1.0f, 0.0f,   (float)type,   u_max, v_max,   faceLight,
                            // Triângulo 2
                            x + offset, y - offset, z + offset,   0.0f, -1.0f, 0.0f,   (float)type,   u_max, v_min,   faceLight,
                            x - offset, y - offset, z - offset,   0.0f, -1.0f, 0.0f,   (float)type,   u_min, v_max,   faceLight,
                            x - offset, y - offset, z + offset,   0.0f, -1.0f, 0.0f,   (float)type,   u_min, v_min,   faceLight,
                        });
                }
                
                // FACE DIREITA (+X) - verificar bloco à direita
                int blockRight = world.getBlockType(Vec3i{globalX + 1, globalY, globalZ});
                if (blockRight == -1) blockRight = ID_AR;
                if (blockRight == ID_AR || 
                    blockRight == ID_AGUA || 
                    getBlockRenderType(blockRight) == TRANSLUCENT || 
                    getBlockRenderType(blockRight) == CROSS) {
                        float u_min = blockInfo.tex_side.x * uv_x_step;
                        float v_min = blockInfo.tex_side.y * uv_y_step;
                        float u_max = u_min + uv_x_step;
                        float v_max = v_min + uv_y_step;
                        float faceLight = world.isExposedToSky(globalX + 1, globalY, globalZ) ? 0.866 : baseLight;
                        if (type == ID_PEDRA) faceLight -= 0.4;
                        mesh.insert(mesh.end(), {
                            // Triângulo 1
                            x + offset, y - offset, z - offset,   1.0f, 0.0f, 0.0f,   (float)type,   u_min, v_max,   faceLight,
                            x + offset, y - offset, z + offset,   1.0f, 0.0f, 0.0f,   (float)type,   u_max, v_max,   faceLight,
                            x + offset, y + offset, z + offset,   1.0f, 0.0f, 0.0f,   (float)type,   u_max, v_min,   faceLight,
                            // Triângulo 2
                            x + offset, y + offset, z + offset,   1.0f, 0.0f, 0.0f,   (float)type,   u_max, v_min,   faceLight,
                            x + offset, y + offset, z - offset,   1.0f, 0.0f, 0.0f,   (float)type,   u_min, v_min,   faceLight,
                            x + offset, y - offset, z - offset,   1.0f, 0.0f, 0.0f,   (float)type,   u_min, v_max,   faceLight,
                        });
                }
                
                // FACE ESQUERDA (-X) - verificar bloco à esquerda
                int blockLeft = world.getBlockType(Vec3i{globalX - 1, globalY, globalZ});
                if (blockLeft == -1) blockLeft = ID_AR;
                if (blockLeft == ID_AR || 
                    blockLeft == ID_AGUA || 
                    getBlockRenderType(blockLeft) == TRANSLUCENT || 
                    getBlockRenderType(blockLeft) == CROSS) {
                        float u_min = blockInfo.tex_side.x * uv_x_step;
                        float v_min = blockInfo.tex_side.y * uv_y_step;
                        float u_max = u_min + uv_x_step;
                        float v_max = v_min + uv_y_step;
                        float faceLight = world.isExposedToSky(globalX - 1, globalY, globalZ) ? 0.86 : baseLight;
                        if (type == ID_PEDRA) faceLight -= 0.4;
                        mesh.insert(mesh.end(), {
                            // Triângulo 1
                            x - offset, y - offset, z - offset,  -1.0f, 0.0f, 0.0f,   (float)type,   u_max, v_max,   faceLight,
                            x - offset, y + offset, z + offset,  -1.0f, 0.0f, 0.0f,   (float)type,   u_min, v_min,   faceLight,
                            x - offset, y - offset, z + offset,  -1.0f, 0.0f, 0.0f,   (float)type,   u_min, v_max,   faceLight,
                            // Triângulo 2
                            x - offset, y + offset, z + offset,  -1.0f, 0.0f, 0.0f,   (float)type,   u_min, v_min,   faceLight,
                            x - offset, y - offset, z - offset,  -1.0f, 0.0f, 0.0f,   (float)type,   u_max, v_max,   faceLight,
                            x - offset, y + offset, z - offset,  -1.0f, 0.0f, 0.0f,   (float)type,   u_max, v_min,   faceLight,
                        });
                }
                
                // FACE FRENTE (+Z) - verificar bloco na frente
                int blockFront = world.getBlockType(Vec3i{globalX, globalY, globalZ + 1});
                if (blockFront == -1) blockFront = ID_AR;
                if (blockFront == ID_AR || 
                    blockFront == ID_AGUA || 
                    getBlockRenderType(blockFront) == TRANSLUCENT ||
                    getBlockRenderType(blockFront) == CROSS) {
                        float u_min = blockInfo.tex_side.x * uv_x_step;
                        float v_min = blockInfo.tex_side.y * uv_y_step;
                        float u_max = u_min + uv_x_step;
                        float v_max = v_min + uv_y_step;
                        float faceLight = world.isExposedToSky(globalX, globalY, globalZ + 1) ? 0.86 : baseLight;
                        if (type == ID_PEDRA) faceLight -= 0.4;
                        mesh.insert(mesh.end(), {
                            // Triângulo 1
                            x - offset, y - offset, z + offset,   0.0f, 0.0f, 1.0f,   (float)type,   u_max, v_max,   faceLight,
                            x + offset, y - offset, z + offset,   0.0f, 0.0f, 1.0f,   (float)type,   u_min, v_max,   faceLight,
                            x + offset, y + offset, z + offset,   0.0f, 0.0f, 1.0f,   (float)type,   u_min, v_min,   faceLight,
                            // Triângulo 2
                            x + offset, y + offset, z + offset,   0.0f, 0.0f, 1.0f,   (float)type,   u_min, v_min,   faceLight,
                            x - offset, y + offset, z + offset,   0.0f, 0.0f, 1.0f,   (float)type,   u_max, v_min,   faceLight,
                            x - offset, y - offset, z + offset,   0.0f, 0.0f, 1.0f,   (float)type,   u_max, v_max,   faceLight,
                        });
                }
                
                // FACE TRÁS (-Z) - verificar bloco atrás
                int blockBack = world.getBlockType(Vec3i{globalX, globalY, globalZ - 1});
                if (blockBack == -1) blockBack = ID_AR;
                if (blockBack == ID_AR || 
                    blockBack == ID_AGUA || 
                    getBlockRenderType(blockBack) == TRANSLUCENT ||
                    getBlockRenderType(blockBack) == CROSS) {
                        float u_min = blockInfo.tex_side.x * uv_x_step;
                        float v_min = blockInfo.tex_side.y * uv_y_step;
                        float u_max = u_min + uv_x_step;
                        float v_max = v_min + uv_y_step;
                        float faceLight = world.isExposedToSky(globalX, globalY, globalZ - 1) ? 0.86 : baseLight;
                        if (type == ID_PEDRA) faceLight -= 0.4;
                        mesh.insert(mesh.end(), {
                            // Triângulo 1
                            x - offset, y - offset, z - offset,   0.0f, 0.0f, -1.0f,   (float)type,   u_min, v_max,   faceLight,
                            x + offset, y + offset, z - offset,   0.0f, 0.0f, -1.0f,   (float)type,   u_max, v_min,   faceLight,
                            x + offset, y - offset, z - offset,   0.0f, 0.0f, -1.0f,   (float)type,   u_max, v_max,   faceLight,
                            // Triângulo 2
                            x + offset, y + offset, z - offset,   0.0f, 0.0f, -1.0f,   (float)type,   u_max, v_min,   faceLight,
                            x - offset, y - offset, z - offset,   0.0f, 0.0f, -1.0f,   (float)type,   u_min, v_max,   faceLight,
                            x - offset, y + offset, z - offset,   0.0f, 0.0f, -1.0f,   (float)type,   u_min, v_min,   faceLight,
                        });
                }
            }
        }
    }
    if (!mesh.empty()) {
        solidCount = mesh.size() / 10;
        glBindBuffer(GL_ARRAY_BUFFER, solid_VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(float), mesh.data(), GL_STATIC_DRAW);
    } else {
        solidCount = 0;
    }
}

void Chunk::buildTransMesh(const World &world, const Vec3i &chunkPos) {
    if (trans_VAO == 0) {
        glGenVertexArrays(1, &trans_VAO);
        glGenBuffers(1, &trans_VBO);

        glBindVertexArray(trans_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, trans_VBO);

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
    }
    std::vector<float> mesh;
    float baseLight = 0.1;
    float offset = 0.5f;
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_DEPTH; z++) {
                int type = blocks[x][y][z];
                if (getBlockRenderType(type) != TRANSLUCENT) continue;
                int globalX = chunkPos.x * CHUNK_WIDTH + x;
                int globalY = chunkPos.y * CHUNK_HEIGHT + y;
                int globalZ = chunkPos.z * CHUNK_DEPTH + z;
                const Block &blockInfo = world.getBlockInfo(type);
                float atlasW = 1024.0f;
                float atlasH = 512.0f;
                float tileW = 16.0f;
                float tileH = 16.0f;
                float uv_x_step = tileW / atlasW;
                float uv_y_step = tileH / atlasH;

                // FACE CIMA (+Y) - verificar bloco acima
                int blockAbove = world.getBlockType(Vec3i{globalX, globalY + 1, globalZ});
                if (blockAbove == -1) blockAbove = 0;
                if (blockAbove != type && (blockAbove == ID_AR || blockAbove == ID_AGUA)) {
                    float u_min = blockInfo.tex_top.x * uv_x_step;
                    float v_min = blockInfo.tex_top.y * uv_y_step;
                    float u_max = u_min + uv_x_step;
                    float v_max = v_min + uv_y_step;
                    float faceLight = world.isExposedToSky(globalX, globalY + 1, globalZ) ? 1.0 : baseLight;
                    mesh.insert(mesh.end(), {
                        // Triângulo 1
                        x - offset, y + offset, z - offset,   0.0f, 1.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
                        x + offset, y + offset, z - offset,   0.0f, 1.0f, 0.0f,   (float)type,        u_max, v_max,     faceLight,
                        x + offset, y + offset, z + offset,   0.0f, 1.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        // Triângulo 2
                        x + offset, y + offset, z + offset,   0.0f, 1.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        x - offset, y + offset, z + offset,   0.0f, 1.0f, 0.0f,   (float)type,        u_min, v_min,     faceLight,
                        x - offset, y + offset, z - offset,   0.0f, 1.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
                    });
                }

                // FACE BAIXO (-Y) - verificar bloco debaixo
                int blockBelow = world.getBlockType(Vec3i{globalX, globalY - 1, globalZ});
                if (blockBelow == -1) blockBelow = 0;
                if (blockBelow != type && (blockBelow == ID_AR || blockBelow == ID_AGUA)) {
                    float u_min = blockInfo.tex_bottom.x * uv_x_step;
                    float v_min = blockInfo.tex_bottom.y * uv_y_step;
                    float u_max = u_min + uv_x_step;
                    float v_max = v_min + uv_y_step;
                    float faceLight = world.isExposedToSky(globalX, globalY - 1, globalZ) ? 0.5 : baseLight;
                    mesh.insert(mesh.end(), {
                        // Triângulo 1
                        x - offset, y - offset, z - offset,   0.0f,-1.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        x + offset, y - offset, z + offset,   0.0f,-1.0f, 0.0f,   (float)type,        u_min, v_min,     faceLight,
                        x + offset, y - offset, z - offset,   0.0f,-1.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
                        // Triângulo 2                        
                        x + offset, y - offset, z + offset,   0.0f,-1.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
                        x - offset, y - offset, z - offset,   0.0f,-1.0f, 0.0f,   (float)type,        u_max, v_max,     faceLight,
                        x - offset, y - offset, z + offset,   0.0f,-1.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                    });
                }
                
                // FACE DIREITA (+X) - verificar bloco à direita
                int blockRight = world.getBlockType(Vec3i{globalX + 1, globalY, globalZ});
                if (blockRight == -1) blockRight = 0;
                if (blockRight != type && (blockRight == ID_AR || blockRight == ID_AGUA)) {
                    float u_min = blockInfo.tex_side.x * uv_x_step;
                    float v_min = blockInfo.tex_side.y * uv_y_step;
                    float u_max = u_min + uv_x_step;
                    float v_max = v_min + uv_y_step;
                    float faceLight = world.isExposedToSky(globalX + 1, globalY, globalZ) ? 0.8 : baseLight;
                    mesh.insert(mesh.end(), {
                        // Triângulo 1
                        x + offset, y - offset, z - offset,   1.0f, 0.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
                        x + offset, y - offset, z + offset,   1.0f, 0.0f, 0.0f,   (float)type,        u_max, v_max,     faceLight,
                        x + offset, y + offset, z + offset,   1.0f, 0.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        // Triângulo 2
                        x + offset, y + offset, z + offset,   1.0f, 0.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        x + offset, y + offset, z - offset,   1.0f, 0.0f, 0.0f,   (float)type,        u_min, v_min,     faceLight,
                        x + offset, y - offset, z - offset,   1.0f, 0.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
                    });
                }
                
                // FACE ESQUERDA (-X) - verificar bloco à esquerda
                int blockLeft = world.getBlockType(Vec3i{globalX - 1, globalY, globalZ});
                if (blockLeft == -1) blockLeft = 0;
                if (blockLeft != type && (blockLeft == ID_AR || blockLeft == ID_AGUA)) {
                    float u_min = blockInfo.tex_side.x * uv_x_step;
                    float v_min = blockInfo.tex_side.y * uv_y_step;
                    float u_max = u_min + uv_x_step;
                    float v_max = v_min + uv_y_step;
                    float faceLight = world.isExposedToSky(globalX - 1, globalY, globalZ) ? 0.8 : baseLight;
                    mesh.insert(mesh.end(), {
                        // Triângulo 1
                        x - offset, y - offset, z - offset,  -1.0f, 0.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
                        x - offset, y + offset, z + offset,  -1.0f, 0.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        x - offset, y - offset, z + offset,  -1.0f, 0.0f, 0.0f,   (float)type,        u_max, v_max,     faceLight,
                        // Triângulo 2
                        x - offset, y + offset, z + offset,  -1.0f, 0.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        x - offset, y - offset, z - offset,  -1.0f, 0.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
                        x - offset, y + offset, z - offset,  -1.0f, 0.0f, 0.0f,   (float)type,        u_min, v_min,     faceLight,
                    });
                }
                
                // FACE FRENTE (+Z) - verificar bloco na frente
                int blockFront = world.getBlockType(Vec3i{globalX, globalY, globalZ + 1});
                if (blockFront == -1) blockFront = 0;
                if (blockFront != type && (blockFront == ID_AR || blockFront == ID_AGUA)) {
                    float u_min = blockInfo.tex_side.x * uv_x_step;
                    float v_min = blockInfo.tex_side.y * uv_y_step;
                    float u_max = u_min + uv_x_step;
                    float v_max = v_min + uv_y_step;
                    float faceLight = world.isExposedToSky(globalX, globalY, globalZ + 1) ? 0.8 : baseLight;
                    mesh.insert(mesh.end(), {
                        // Triângulo 1
                        x - offset, y - offset, z + offset,   0.0f, 0.0f, 1.0f,   (float)type,        u_min, v_max,     faceLight,
                        x + offset, y - offset, z + offset,   0.0f, 0.0f, 1.0f,   (float)type,        u_max, v_max,     faceLight,
                        x + offset, y + offset, z + offset,   0.0f, 0.0f, 1.0f,   (float)type,        u_max, v_min,     faceLight,
                        // Triângulo 2
                        x + offset, y + offset, z + offset,   0.0f, 0.0f, 1.0f,   (float)type,        u_max, v_min,     faceLight,
                        x - offset, y + offset, z + offset,   0.0f, 0.0f, 1.0f,   (float)type,        u_min, v_min,     faceLight,
                        x - offset, y - offset, z + offset,   0.0f, 0.0f, 1.0f,   (float)type,        u_min, v_max,     faceLight,
                    });
                }
                
                // FACE TRÁS (-Z) - verificar bloco atrás
                int blockBack = world.getBlockType(Vec3i{globalX, globalY, globalZ - 1});
                if (blockBack == -1) blockBack = 0;
                if (blockBack != type && (blockBack == ID_AR || blockBack == ID_AGUA)) {
                    float u_min = blockInfo.tex_side.x * uv_x_step;
                    float v_min = blockInfo.tex_side.y * uv_y_step;
                    float u_max = u_min + uv_x_step;
                    float v_max = v_min + uv_y_step;
                    float faceLight = world.isExposedToSky(globalX, globalY, globalZ - 1) ? 0.8 : baseLight;
                    mesh.insert(mesh.end(), {
                        // Triângulo 1
                        x - offset, y - offset, z - offset,   0.0f, 0.0f,-1.0f,   (float)type,        u_max, v_max,     faceLight,
                        x + offset, y + offset, z - offset,   0.0f, 0.0f,-1.0f,   (float)type,        u_min, v_min,     faceLight,
                        x + offset, y - offset, z - offset,   0.0f, 0.0f,-1.0f,   (float)type,        u_min, v_max,     faceLight,
                        // Triângulo 2
                        x + offset, y + offset, z - offset,   0.0f, 0.0f,-1.0f,   (float)type,        u_min, v_min,     faceLight,
                        x - offset, y - offset, z - offset,   0.0f, 0.0f,-1.0f,   (float)type,        u_max, v_max,     faceLight,
                        x - offset, y + offset, z - offset,   0.0f, 0.0f,-1.0f,   (float)type,        u_max, v_min,     faceLight,
                    });
                }
            }
        }
    }
    if (!mesh.empty()) {
        transCount = mesh.size() / 10;
        glBindBuffer(GL_ARRAY_BUFFER, trans_VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(float), mesh.data(), GL_STATIC_DRAW);
    } else {
        transCount = 0;
    }
}

void Chunk::buildWaterMesh(const World &world, const Vec3i &chunkPos) {
    if (water_VAO == 0) {
        glGenVertexArrays(1, &water_VAO);
        glGenBuffers(1, &water_VBO);

        glBindVertexArray(water_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, water_VBO);

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
    }
    std::vector<float> mesh;
    float baseLight = 0.1;
    float offset = 0.5f;
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_DEPTH; z++) {
                int type = blocks[x][y][z];
                if (getBlockRenderType(type) != WATER) continue;
                int globalX = chunkPos.x * CHUNK_WIDTH + x;
                int globalY = chunkPos.y * CHUNK_HEIGHT + y;
                int globalZ = chunkPos.z * CHUNK_DEPTH + z;
                const Block &blockInfo = world.getBlockInfo(type);
                float atlasW = 1024.0f;
                float atlasH = 512.0f;
                float tileW = 16.0f;
                float tileH = 16.0f;
                float uv_x_step = tileW / atlasW;
                float uv_y_step = tileH / atlasH;

                // FACE CIMA (+Y) - verificar bloco acima
                int blockAbove = world.getBlockType(Vec3i{globalX, globalY + 1, globalZ});
                if (blockAbove == -1) blockAbove = 0;
                if (blockAbove != ID_AGUA) {
                    float u_min = blockInfo.tex_top.x * uv_x_step;
                    float v_min = blockInfo.tex_top.y * uv_y_step;
                    float u_max = u_min + uv_x_step;
                    float v_max = v_min + uv_y_step;
                    float faceLight = world.isExposedToSky(globalX, globalY + 1, globalZ) ? 1.0 : baseLight;
                    mesh.insert(mesh.end(), {
                        // Triângulo 1
                        x - offset, y + 0.4f, z - offset,   0.0f, 1.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
                        x + offset, y + 0.4f, z - offset,   0.0f, 1.0f, 0.0f,   (float)type,        u_max, v_max,     faceLight,
                        x + offset, y + 0.4f, z + offset,   0.0f, 1.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        // Triângulo 2
                        x + offset, y + 0.4f, z + offset,   0.0f, 1.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        x - offset, y + 0.4f, z + offset,   0.0f, 1.0f, 0.0f,   (float)type,        u_min, v_min,     faceLight,
                        x - offset, y + 0.4f, z - offset,   0.0f, 1.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
                    });
                }

                // FACE BAIXO (-Y) - verificar bloco debaixo
                int blockBelow = world.getBlockType(Vec3i{globalX, globalY - 1, globalZ});
                if (blockBelow == -1) blockBelow = 0;
                if (blockBelow != ID_AGUA) {
                    float u_min = blockInfo.tex_bottom.x * uv_x_step;
                    float v_min = blockInfo.tex_bottom.y * uv_y_step;
                    float u_max = u_min + uv_x_step;
                    float v_max = v_min + uv_y_step;
                    float faceLight = world.isExposedToSky(globalX, globalY - 1, globalZ) ? offset: baseLight;
                    mesh.insert(mesh.end(), {
                        // Triângulo 1
                        x - offset, y - offset, z - offset,   0.0f,-1.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
                        x + offset, y - offset, z + offset,   0.0f,-1.0f, 0.0f,   (float)type,        u_max, v_max,     faceLight,
                        x + offset, y - offset, z - offset,   0.0f,-1.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        // Triângulo 2
                        x + offset, y - offset, z + offset,   0.0f,-1.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        x - offset, y - offset, z - offset,   0.0f,-1.0f, 0.0f,   (float)type,        u_min, v_min,     faceLight,
                        x - offset, y - offset, z + offset,   0.0f,-1.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
                    });
                }
                
                // FACE DIREITA (+X) - verificar bloco à direita
                int blockRight = world.getBlockType(Vec3i{globalX + 1, globalY, globalZ});
                if (blockRight == -1) blockRight = 0;
                if (blockRight != ID_AGUA) {
                    float u_min = blockInfo.tex_side.x * uv_x_step;
                    float v_min = blockInfo.tex_side.y * uv_y_step;
                    float u_max = u_min + uv_x_step;
                    float v_max = v_min + uv_y_step;
                    float faceLight = world.isExposedToSky(globalX + 1, globalY, globalZ) ? 0.8 : baseLight;
                    mesh.insert(mesh.end(), {
                        // Triângulo 1
                        x + offset, y - offset, z - offset,   1.0f, 0.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
                        x + offset, y - offset, z + offset,   1.0f, 0.0f, 0.0f,   (float)type,        u_max, v_max,     faceLight,
                        x + offset, y + offset, z + offset,   1.0f, 0.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        // Triângulo 2
                        x + offset, y + offset, z + offset,   1.0f, 0.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        x + offset, y + offset, z - offset,   1.0f, 0.0f, 0.0f,   (float)type,        u_min, v_min,     faceLight,
                        x + offset, y - offset, z - offset,   1.0f, 0.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
                    });
                }
                
                // FACE ESQUERDA (-X) - verificar bloco à esquerda
                int blockLeft = world.getBlockType(Vec3i{globalX - 1, globalY, globalZ});
                if (blockLeft == -1) blockLeft = 0;
                if (blockLeft != ID_AGUA) {
                    float u_min = blockInfo.tex_side.x * uv_x_step;
                    float v_min = blockInfo.tex_side.y * uv_y_step;
                    float u_max = u_min + uv_x_step;
                    float v_max = v_min + uv_y_step;
                    float faceLight = world.isExposedToSky(globalX - 1, globalY, globalZ) ? 0.8 : baseLight;
                    mesh.insert(mesh.end(), {
                        // Triângulo 1
                        x - offset, y - offset, z - offset,  -1.0f, 0.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
                        x - offset, y + offset, z + offset,  -1.0f, 0.0f, 0.0f,   (float)type,        u_max, v_max,     faceLight,
                        x - offset, y - offset, z + offset,  -1.0f, 0.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        // Triângulo 2
                        x - offset, y + offset, z + offset,  -1.0f, 0.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        x - offset, y - offset, z - offset,  -1.0f, 0.0f, 0.0f,   (float)type,        u_min, v_min,     faceLight,
                        x - offset, y + offset, z - offset,  -1.0f, 0.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
                    });
                }
                
                // FACE FRENTE (+Z) - verificar bloco na frente
                int blockFront = world.getBlockType(Vec3i{globalX, globalY, globalZ + 1});
                if (blockFront == -1) blockFront = 0;
                if (blockFront != ID_AGUA) {
                    float u_min = blockInfo.tex_side.x * uv_x_step;
                    float v_min = blockInfo.tex_side.y * uv_y_step;
                    float u_max = u_min + uv_x_step;
                    float v_max = v_min + uv_y_step;
                    float faceLight = world.isExposedToSky(globalX, globalY, globalZ + 1) ? 0.8 : baseLight;
                    mesh.insert(mesh.end(), {
                        // Triângulo 1
                        x - offset, y - offset, z + offset,   0.0f, 0.0f, 1.0f,   (float)type,        u_min, v_max,     faceLight,
                        x + offset, y - offset, z + offset,   0.0f, 0.0f, 1.0f,   (float)type,        u_max, v_max,     faceLight,
                        x + offset, y + offset, z + offset,   0.0f, 0.0f, 1.0f,   (float)type,        u_max, v_min,     faceLight,
                        // Triângulo 2
                        x + offset, y + offset, z + offset,   0.0f, 0.0f, 1.0f,   (float)type,        u_max, v_min,     faceLight,
                        x - offset, y + offset, z + offset,   0.0f, 0.0f, 1.0f,   (float)type,        u_min, v_min,     faceLight,
                        x - offset, y - offset, z + offset,   0.0f, 0.0f, 1.0f,   (float)type,        u_min, v_max,     faceLight,
                    });
                }
                
                // FACE TRÁS (-Z) - verificar bloco atrás
                int blockBack = world.getBlockType(Vec3i{globalX, globalY, globalZ - 1});
                if (blockBack == -1) blockBack = 0;
                if (blockBack != ID_AGUA) {
                    float u_min = blockInfo.tex_side.x * uv_x_step;
                    float v_min = blockInfo.tex_side.y * uv_y_step;
                    float u_max = u_min + uv_x_step;
                    float v_max = v_min + uv_y_step;
                    float faceLight = world.isExposedToSky(globalX, globalY, globalZ - 1) ? 0.8 : baseLight;
                    mesh.insert(mesh.end(), {
                        // Triângulo 1
                        x - offset, y - offset, z - offset,   0.0f, 0.0f,-1.0f,   (float)type,        u_min, v_max,     faceLight,
                        x + offset, y + offset, z - offset,   0.0f, 0.0f,-1.0f,   (float)type,        u_max, v_max,     faceLight,
                        x + offset, y - offset, z - offset,   0.0f, 0.0f,-1.0f,   (float)type,        u_max, v_min,     faceLight,
                        // Triângulo 2
                        x + offset, y + offset, z - offset,   0.0f, 0.0f,-1.0f,   (float)type,        u_max, v_min,     faceLight,
                        x - offset, y - offset, z - offset,   0.0f, 0.0f,-1.0f,   (float)type,        u_min, v_min,     faceLight,
                        x - offset, y + offset, z - offset,   0.0f, 0.0f,-1.0f,   (float)type,        u_min, v_max,     faceLight,
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

void Chunk::buildCrossMesh(const World &world, const Vec3i &chunkPos) {
if (cross_VAO == 0) {
        glGenVertexArrays(1, &cross_VAO);
        glGenBuffers(1, &cross_VBO);

        glBindVertexArray(cross_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, cross_VBO);

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
    }
    
    std::vector<float> mesh;
    float baseLight = 0.9;
    float offset = 0.5f;
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_DEPTH; z++) {
                int type = blocks[x][y][z];
                if (getBlockRenderType(type) != CROSS) continue;
                
                int globalX = chunkPos.x * CHUNK_WIDTH + x;
                int globalY = chunkPos.y * CHUNK_HEIGHT + y;
                int globalZ = chunkPos.z * CHUNK_DEPTH + z;
                
                // Verificar se bloco abaixo é sólido (flores precisam de base)
                int blockBelow = world.getBlockType(Vec3i{globalX, globalY - 1, globalZ});
                if (blockBelow == ID_AR || getBlockRenderType(blockBelow) != SOLID) continue;
                
                const Block &blockInfo = world.getBlockInfo(type);
                float atlasW = 1024.0f;
                float atlasH = 512.0f;
                float tileW = 16.0f;
                float tileH = 16.0f;
                float uv_x_step = tileW / atlasW;
                float uv_y_step = tileH / atlasH;
                
                float u_min = blockInfo.tex_top.x * uv_x_step;
                float v_min = blockInfo.tex_top.y * uv_y_step;
                float u_max = u_min + uv_x_step;
                float v_max = v_min + uv_y_step;
                
                float faceLight = world.isExposedToSky(globalX, globalY, globalZ) ? 1.0 : baseLight;
                
                // CROSS SHAPE: 2 planos perpendiculares
                
                // Plano 1: diagonal NE-SW
                mesh.insert(mesh.end(), {
                    // Face 1
                    x - offset, y - offset, z - offset,   0.707f, 0.0f, 0.707f,   (float)type,   u_min, v_max,   faceLight,
                    x + offset, y - offset, z + offset,   0.707f, 0.0f, 0.707f,   (float)type,   u_max, v_max,   faceLight,
                    x + offset, y + offset, z + offset,   0.707f, 0.0f, 0.707f,   (float)type,   u_max, v_min,   faceLight,
                    
                    x + offset, y + offset, z + offset,   0.707f, 0.0f, 0.707f,   (float)type,   u_max, v_min,   faceLight,
                    x - offset, y + offset, z - offset,   0.707f, 0.0f, 0.707f,   (float)type,   u_min, v_min,   faceLight,
                    x - offset, y - offset, z - offset,   0.707f, 0.0f, 0.707f,   (float)type,   u_min, v_max,   faceLight,
                    
                    // Face 2 (lado oposto)
                    x - offset, y - offset, z - offset,  -0.707f, 0.0f,-0.707f,   (float)type,   u_min, v_max,   faceLight,
                    x + offset, y + offset, z + offset,  -0.707f, 0.0f,-0.707f,   (float)type,   u_max, v_min,   faceLight,
                    x + offset, y - offset, z + offset,  -0.707f, 0.0f,-0.707f,   (float)type,   u_max, v_max,   faceLight,
                    
                    x + offset, y + offset, z + offset,  -0.707f, 0.0f,-0.707f,   (float)type,   u_max, v_min,   faceLight,
                    x - offset, y - offset, z - offset,  -0.707f, 0.0f,-0.707f,   (float)type,   u_min, v_max,   faceLight,
                    x - offset, y + offset, z - offset,  -0.707f, 0.0f,-0.707f,   (float)type,   u_min, v_min,   faceLight,
                });
                
                // Plano 2: diagonal NW-SE
                mesh.insert(mesh.end(), {
                    // Face 1
                    x - offset, y - offset, z + offset,  -0.707f, 0.0f, 0.707f,   (float)type,   u_min, v_max,   faceLight,
                    x + offset, y - offset, z - offset,  -0.707f, 0.0f, 0.707f,   (float)type,   u_max, v_max,   faceLight,
                    x + offset, y + offset, z - offset,  -0.707f, 0.0f, 0.707f,   (float)type,   u_max, v_min,   faceLight,
                    
                    x + offset, y + offset, z - offset,  -0.707f, 0.0f, 0.707f,   (float)type,   u_max, v_min,   faceLight,
                    x - offset, y + offset, z + offset,  -0.707f, 0.0f, 0.707f,   (float)type,   u_min, v_min,   faceLight,
                    x - offset, y - offset, z + offset,  -0.707f, 0.0f, 0.707f,   (float)type,   u_min, v_max,   faceLight,
                    
                    // Face 2 (lado oposto)
                    x - offset, y - offset, z + offset,   0.707f, 0.0f,-0.707f,   (float)type,   u_min, v_max,   faceLight,
                    x + offset, y + offset, z - offset,   0.707f, 0.0f,-0.707f,   (float)type,   u_max, v_min,   faceLight,
                    x + offset, y - offset, z - offset,   0.707f, 0.0f,-0.707f,   (float)type,   u_max, v_max,   faceLight,
                    
                    x + offset, y + offset, z - offset,   0.707f, 0.0f,-0.707f,   (float)type,   u_max, v_min,   faceLight,
                    x - offset, y - offset, z + offset,   0.707f, 0.0f,-0.707f,   (float)type,   u_min, v_max,   faceLight,
                    x - offset, y + offset, z + offset,   0.707f, 0.0f,-0.707f,   (float)type,   u_min, v_min,   faceLight,
                });
            }
        }
    }
    
    if (!mesh.empty()) {
        crossCount = mesh.size() / 10;
        glBindBuffer(GL_ARRAY_BUFFER, cross_VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(float), mesh.data(), GL_STATIC_DRAW);
    } else {
        crossCount = 0;
    }
}

void Chunk::drawSolid() {
    if (solidCount == 0) return;
    glBindVertexArray(solid_VAO);
    glDrawArrays(GL_TRIANGLES, 0, solidCount);
    glBindVertexArray(0);
}


void Chunk::drawTrans() {
    if (transCount == 0) return;
    glBindVertexArray(trans_VAO);
    glDrawArrays(GL_TRIANGLES, 0, transCount);
    glBindVertexArray(0);
}


void Chunk::drawWater() {
    if (waterCount == 0) return;
    glBindVertexArray(water_VAO);
    glDrawArrays(GL_TRIANGLES, 0, waterCount);
    glBindVertexArray(0);
}

void Chunk::drawCross() {
    if (crossCount == 0) return;
    glBindVertexArray(cross_VAO);
    glDrawArrays(GL_TRIANGLES, 0, crossCount);
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
                if (getBlockRenderType(blocks[x][y][z]) == SOLID) {
                    heightMap[x][z] = y;
                    break;
                }
            }
        }
    }
}

void Chunk::setPosition(const Vec3i& pos) {
    this->pos = pos;
}

Vec3i Chunk::getPosition() const {
    return pos;
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