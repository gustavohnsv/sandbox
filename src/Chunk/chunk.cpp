#include "chunk.h"

#include "../World/world.h"

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
    auto start_file = std::chrono::high_resolution_clock::now();
    std::ifstream input(filepath, std::ios::binary);
    if (!input) {
        return false;
    }
    input.read(reinterpret_cast<char*>(blocks), sizeof(blocks));
    input.close();
    updateHeightMap();
    // buildMesh(world, chunkPos);
    // buildWaterMesh(world, chunkPos);

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
                if (type == ID_AR || type == ID_AGUA) continue;
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

                int pumpkinFace = -1;
                if (type == ID_ABOBORA) {
                    pumpkinFace = rand() % 4;
                }
                
                // FACE CIMA (+Y) - verificar bloco acima
                int blockAbove = world.getBlockType(Vec3i{globalX, globalY + 1, globalZ});
                if (blockAbove == -1) blockAbove = ID_AR;
                if (blockAbove == ID_AR || blockAbove == ID_AGUA) {
                    float u_min = blockInfo.tex_top.x * uv_x_step;
                    float v_min = blockInfo.tex_top.y * uv_y_step;
                    float u_max = u_min + uv_x_step;
                    float v_max = v_min + uv_y_step;
                    float faceLight = world.isExposedToSky(globalX, globalY + 1, globalZ) ? 1.0 : baseLight;
                    if (type == ID_PEDRA) faceLight -= 0.4; 
                    mesh.insert(mesh.end(), {
                        // Triângulo 1
                        x - 0.5f, y + 0.5f, z - 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,   u_min, v_min,   faceLight,
                        x + 0.5f, y + 0.5f, z - 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,   u_max, v_min,   faceLight,
                        x + 0.5f, y + 0.5f, z + 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,   u_max, v_max,   faceLight,
                        // Triângulo 2
                        x + 0.5f, y + 0.5f, z + 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,   u_max, v_max,   faceLight,
                        x - 0.5f, y + 0.5f, z + 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,   u_min, v_max,   faceLight,
                        x - 0.5f, y + 0.5f, z - 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,   u_min, v_min,   faceLight,
                    });
                }

                // FACE BAIXO (-Y) - verificar bloco debaixo
                int blockBelow = world.getBlockType(Vec3i{globalX, globalY - 1, globalZ});
                if (blockBelow == -1) blockBelow = ID_AR;
                if (blockBelow == ID_AR || blockBelow == ID_AGUA) {
                    float u_min = blockInfo.tex_bottom.x * uv_x_step;
                    float v_min = blockInfo.tex_bottom.y * uv_y_step;
                    float u_max = u_min + uv_x_step;
                    float v_max = v_min + uv_y_step;
                    float faceLight = world.isExposedToSky(globalX, globalY - 1, globalZ) ? 0.5 : baseLight;
                    if (type == ID_PEDRA) faceLight -= 0.4;
                    mesh.insert(mesh.end(), {
                        // Triângulo 1
                        x - 0.5f, y - 0.5f, z - 0.5f,   0.0f, -1.0f, 0.0f,   (float)type,   u_min, v_max,   faceLight,
                        x + 0.5f, y - 0.5f, z + 0.5f,   0.0f, -1.0f, 0.0f,   (float)type,   u_max, v_min,   faceLight,
                        x + 0.5f, y - 0.5f, z - 0.5f,   0.0f, -1.0f, 0.0f,   (float)type,   u_max, v_max,   faceLight,
                        // Triângulo 2
                        x + 0.5f, y - 0.5f, z + 0.5f,   0.0f, -1.0f, 0.0f,   (float)type,   u_max, v_min,   faceLight,
                        x - 0.5f, y - 0.5f, z - 0.5f,   0.0f, -1.0f, 0.0f,   (float)type,   u_min, v_max,   faceLight,
                        x - 0.5f, y - 0.5f, z + 0.5f,   0.0f, -1.0f, 0.0f,   (float)type,   u_min, v_min,   faceLight,
                    });
                }
                
                // FACE DIREITA (+X) - verificar bloco à direita
                int blockRight = world.getBlockType(Vec3i{globalX + 1, globalY, globalZ});
                if (blockRight == -1) blockRight = ID_AR;
                if (blockRight == ID_AR || blockRight == ID_AGUA) {
                    float u_min = blockInfo.tex_side.x * uv_x_step;
                    float v_min = blockInfo.tex_side.y * uv_y_step;
                    if (pumpkinFace != -1 && pumpkinFace != 0) {
                        u_min = blockInfo.tex_top.x * uv_x_step;
                        v_min = blockInfo.tex_top.y * uv_y_step;
                    }
                    float u_max = u_min + uv_x_step;
                    float v_max = v_min + uv_y_step;
                    float faceLight = world.isExposedToSky(globalX + 1, globalY, globalZ) ? 0.8 : baseLight;
                    if (type == ID_PEDRA) faceLight -= 0.4;
                    mesh.insert(mesh.end(), {
                        // Triângulo 1
                        x + 0.5f, y - 0.5f, z - 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,   u_min, v_max,   faceLight,
                        x + 0.5f, y - 0.5f, z + 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,   u_max, v_max,   faceLight,
                        x + 0.5f, y + 0.5f, z + 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,   u_max, v_min,   faceLight,
                        // Triângulo 2
                        x + 0.5f, y + 0.5f, z + 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,   u_max, v_min,   faceLight,
                        x + 0.5f, y + 0.5f, z - 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,   u_min, v_min,   faceLight,
                        x + 0.5f, y - 0.5f, z - 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,   u_min, v_max,   faceLight,
                    });
                }
                
                // FACE ESQUERDA (-X) - verificar bloco à esquerda
                int blockLeft = world.getBlockType(Vec3i{globalX - 1, globalY, globalZ});
                if (blockLeft == -1) blockLeft = ID_AR;
                if (blockLeft == ID_AR || blockLeft == ID_AGUA) {
                    float u_min = blockInfo.tex_side.x * uv_x_step;
                    float v_min = blockInfo.tex_side.y * uv_y_step;
                    if (pumpkinFace != -1 && pumpkinFace != 1) {
                        u_min = blockInfo.tex_top.x * uv_x_step;
                        v_min = blockInfo.tex_top.y * uv_y_step;
                    }
                    float u_max = u_min + uv_x_step;
                    float v_max = v_min + uv_y_step;
                    float faceLight = world.isExposedToSky(globalX - 1, globalY, globalZ) ? 0.8 : baseLight;
                    if (type == ID_PEDRA) faceLight -= 0.4;
                    mesh.insert(mesh.end(), {
                        // Triângulo 1
                        x - 0.5f, y - 0.5f, z - 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,   u_max, v_max,   faceLight,
                        x - 0.5f, y + 0.5f, z + 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,   u_min, v_min,   faceLight,
                        x - 0.5f, y - 0.5f, z + 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,   u_min, v_max,   faceLight,
                        // Triângulo 2
                        x - 0.5f, y + 0.5f, z + 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,   u_min, v_min,   faceLight,
                        x - 0.5f, y - 0.5f, z - 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,   u_max, v_max,   faceLight,
                        x - 0.5f, y + 0.5f, z - 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,   u_max, v_min,   faceLight,
                    });
                }
                
                // FACE FRENTE (+Z) - verificar bloco na frente
                int blockFront = world.getBlockType(Vec3i{globalX, globalY, globalZ + 1});
                if (blockFront == -1) blockFront = ID_AR;
                if (blockFront == ID_AR || blockFront == ID_AGUA) {
                    float u_min = blockInfo.tex_side.x * uv_x_step;
                    float v_min = blockInfo.tex_side.y * uv_y_step;
                    if (pumpkinFace != -1 && pumpkinFace != 2) {
                        u_min = blockInfo.tex_top.x * uv_x_step;
                        v_min = blockInfo.tex_top.y * uv_y_step;
                    }
                    float u_max = u_min + uv_x_step;
                    float v_max = v_min + uv_y_step;
                    float faceLight = world.isExposedToSky(globalX, globalY, globalZ + 1) ? 0.8 : baseLight;
                    if (type == ID_PEDRA) faceLight -= 0.4;
                    mesh.insert(mesh.end(), {
                        // Triângulo 1
                        x - 0.5f, y - 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,   u_max, v_max,   faceLight,
                        x + 0.5f, y - 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,   u_min, v_max,   faceLight,
                        x + 0.5f, y + 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,   u_min, v_min,   faceLight,
                        // Triângulo 2
                        x + 0.5f, y + 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,   u_min, v_min,   faceLight,
                        x - 0.5f, y + 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,   u_max, v_min,   faceLight,
                        x - 0.5f, y - 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,   u_max, v_max,   faceLight,
                    });
                }
                
                // FACE TRÁS (-Z) - verificar bloco atrás
                int blockBack = world.getBlockType(Vec3i{globalX, globalY, globalZ - 1});
                if (blockBack == -1) blockBack = ID_AR;
                if (blockBack == ID_AR || blockBack == ID_AGUA) {
                    float u_min = blockInfo.tex_side.x * uv_x_step;
                    float v_min = blockInfo.tex_side.y * uv_y_step;
                    if (pumpkinFace != -1 && pumpkinFace != 3) {
                        u_min = blockInfo.tex_top.x * uv_x_step;
                        v_min = blockInfo.tex_top.y * uv_y_step;
                    }
                    float u_max = u_min + uv_x_step;
                    float v_max = v_min + uv_y_step;
                    float faceLight = world.isExposedToSky(globalX, globalY, globalZ - 1) ? 0.8 : baseLight;
                    if (type == ID_PEDRA) faceLight -= 0.4;
                    mesh.insert(mesh.end(), {
                        // Triângulo 1
                        x - 0.5f, y - 0.5f, z - 0.5f,   0.0f, 0.0f, -1.0f,   (float)type,   u_min, v_max,   faceLight,
                        x + 0.5f, y + 0.5f, z - 0.5f,   0.0f, 0.0f, -1.0f,   (float)type,   u_max, v_min,   faceLight,
                        x + 0.5f, y - 0.5f, z - 0.5f,   0.0f, 0.0f, -1.0f,   (float)type,   u_max, v_max,   faceLight,
                        // Triângulo 2
                        x + 0.5f, y + 0.5f, z - 0.5f,   0.0f, 0.0f, -1.0f,   (float)type,   u_max, v_min,   faceLight,
                        x - 0.5f, y - 0.5f, z - 0.5f,   0.0f, 0.0f, -1.0f,   (float)type,   u_min, v_max,   faceLight,
                        x - 0.5f, y + 0.5f, z - 0.5f,   0.0f, 0.0f, -1.0f,   (float)type,   u_min, v_min,   faceLight,
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
                if (blocks[x][y][z] != ID_AGUA) continue;
                int type = ID_AGUA;
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
                        x - 0.5f, y + 0.4f, z - 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
                        x + 0.5f, y + 0.4f, z - 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,        u_max, v_max,     faceLight,
                        x + 0.5f, y + 0.4f, z + 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        // Triângulo 2
                        x + 0.5f, y + 0.4f, z + 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        x - 0.5f, y + 0.4f, z + 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,        u_min, v_min,     faceLight,
                        x - 0.5f, y + 0.4f, z - 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
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
                    float faceLight = world.isExposedToSky(globalX, globalY - 1, globalZ) ? 0.5 : baseLight;
                    mesh.insert(mesh.end(), {
                        // Triângulo 1
                        x - 0.5f, y - 0.5f, z - 0.5f,   0.0f,-1.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
                        x + 0.5f, y - 0.5f, z + 0.5f,   0.0f,-1.0f, 0.0f,   (float)type,        u_max, v_max,     faceLight,
                        x + 0.5f, y - 0.5f, z - 0.5f,   0.0f,-1.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        // Triângulo 2
                        x + 0.5f, y - 0.5f, z + 0.5f,   0.0f,-1.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        x - 0.5f, y - 0.5f, z - 0.5f,   0.0f,-1.0f, 0.0f,   (float)type,        u_min, v_min,     faceLight,
                        x - 0.5f, y - 0.5f, z + 0.5f,   0.0f,-1.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
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
                        x + 0.5f, y - 0.5f, z - 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
                        x + 0.5f, y - 0.5f, z + 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,        u_max, v_max,     faceLight,
                        x + 0.5f, y + 0.5f, z + 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        // Triângulo 2
                        x + 0.5f, y + 0.5f, z + 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        x + 0.5f, y + 0.5f, z - 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,        u_min, v_min,     faceLight,
                        x + 0.5f, y - 0.5f, z - 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
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
                        x - 0.5f, y - 0.5f, z - 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
                        x - 0.5f, y + 0.5f, z + 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,        u_max, v_max,     faceLight,
                        x - 0.5f, y - 0.5f, z + 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        // Triângulo 2
                        x - 0.5f, y + 0.5f, z + 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,        u_max, v_min,     faceLight,
                        x - 0.5f, y - 0.5f, z - 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,        u_min, v_min,     faceLight,
                        x - 0.5f, y + 0.5f, z - 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,        u_min, v_max,     faceLight,
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
                        x - 0.5f, y - 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,        u_min, v_max,     faceLight,
                        x + 0.5f, y - 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,        u_max, v_max,     faceLight,
                        x + 0.5f, y + 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,        u_max, v_min,     faceLight,
                        // Triângulo 2
                        x + 0.5f, y + 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,        u_max, v_min,     faceLight,
                        x - 0.5f, y + 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,        u_min, v_min,     faceLight,
                        x - 0.5f, y - 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,        u_min, v_max,     faceLight,
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
                        x - 0.5f, y - 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   (float)type,        u_min, v_max,     faceLight,
                        x + 0.5f, y + 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   (float)type,        u_max, v_max,     faceLight,
                        x + 0.5f, y - 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   (float)type,        u_max, v_min,     faceLight,
                        // Triângulo 2
                        x + 0.5f, y + 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   (float)type,        u_max, v_min,     faceLight,
                        x - 0.5f, y - 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   (float)type,        u_min, v_min,     faceLight,
                        x - 0.5f, y + 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   (float)type,        u_min, v_max,     faceLight,
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