#include "main.h"

Chunk::Chunk() {
    memset(blocks, 0, sizeof(blocks));
    memset(heightMap, 0, sizeof(heightMap));
    VAO = 0;
    VBO = 0;
    count = 0;
    modified = false;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    int stride = 9 * sizeof(float);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(3);

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

bool Chunk::loadFile(const std::string &filepath) {
    std::ifstream input(filepath, std::ios::binary);
    if (!input) {
        return false;
    }
    input.read(reinterpret_cast<char*>(blocks), sizeof(blocks));
    input.close();
    buildMesh();
    return true;
}

int Chunk::getCount() const {
    return count;
}

void Chunk::buildMesh() {
    std::vector<float> mesh;
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_DEPTH; z++) {
                int type = blocks[x][y][z];
                if (type == 0) continue;
                if ((y == CHUNK_HEIGHT - 1) || (blocks[x][y+1][z] == 0)) {
                    mesh.insert(mesh.end(), {
                        // Posições                     // Normais          // Tipo do bloco    // [U,V]
                        x - 0.5f, y + 0.5f, z - 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,        0.0f, 1.0f,
                        x + 0.5f, y + 0.5f, z - 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,        1.0f, 1.0f,
                        x + 0.5f, y + 0.5f, z + 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,        1.0f, 0.0f,
                        x + 0.5f, y + 0.5f, z + 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,        1.0f, 0.0f,
                        x - 0.5f, y + 0.5f, z + 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,        0.0f, 0.0f,
                        x - 0.5f, y + 0.5f, z - 0.5f,   0.0f, 1.0f, 0.0f,   (float)type,        0.0f, 1.0f,
                    });
                }
                if ((y == 0) || (blocks[x][y-1][z] == 0)) {
                    mesh.insert(mesh.end(), {
                        x - 0.5f, y - 0.5f, z - 0.5f,   0.0f,-1.0f, 0.0f,   (float)type,        0.0f, 1.0f,   
                        x + 0.5f, y - 0.5f, z + 0.5f,   0.0f,-1.0f, 0.0f,   (float)type,        1.0f, 1.0f,
                        x + 0.5f, y - 0.5f, z - 0.5f,   0.0f,-1.0f, 0.0f,   (float)type,        1.0f, 0.0f,
                        x + 0.5f, y - 0.5f, z + 0.5f,   0.0f,-1.0f, 0.0f,   (float)type,        1.0f, 0.0f,
                        x - 0.5f, y - 0.5f, z - 0.5f,   0.0f,-1.0f, 0.0f,   (float)type,        0.0f, 0.0f,
                        x - 0.5f, y - 0.5f, z + 0.5f,   0.0f,-1.0f, 0.0f,   (float)type,        0.0f, 1.0f,
                    });
                }
                if ((x == CHUNK_WIDTH - 1) || (blocks[x+1][y][z] == 0)) {
                    mesh.insert(mesh.end(), {
                        x + 0.5f, y - 0.5f, z - 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,        0.0f, 1.0f,   
                        x + 0.5f, y - 0.5f, z + 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,        1.0f, 1.0f,
                        x + 0.5f, y + 0.5f, z + 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,        1.0f, 0.0f,
                        x + 0.5f, y + 0.5f, z + 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,        1.0f, 0.0f,
                        x + 0.5f, y + 0.5f, z - 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,        0.0f, 0.0f,
                        x + 0.5f, y - 0.5f, z - 0.5f,   1.0f, 0.0f, 0.0f,   (float)type,        0.0f, 1.0f,
                    });
                }
                if ((x == 0) || (blocks[x-1][y][z] == 0)) {
                    mesh.insert(mesh.end(), {
                        x - 0.5f, y - 0.5f, z - 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,        0.0f, 1.0f,
                        x - 0.5f, y + 0.5f, z + 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,        1.0f, 1.0f,
                        x - 0.5f, y - 0.5f, z + 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,        1.0f, 0.0f,
                        x - 0.5f, y + 0.5f, z + 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,        1.0f, 0.0f,
                        x - 0.5f, y - 0.5f, z - 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,        0.0f, 0.0f,
                        x - 0.5f, y + 0.5f, z - 0.5f,  -1.0f, 0.0f, 0.0f,   (float)type,        0.0f, 1.0f,
                    });
                }
                if ((z == CHUNK_DEPTH - 1) || (blocks[x][y][z+1] == 0)) {
                    mesh.insert(mesh.end(), {
                        x - 0.5f, y - 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,        0.0f, 1.0f,
                        x + 0.5f, y - 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,        1.0f, 1.0f,
                        x + 0.5f, y + 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,        1.0f, 0.0f,
                        x + 0.5f, y + 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,        1.0f, 0.0f,
                        x - 0.5f, y + 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,        0.0f, 0.0f,
                        x - 0.5f, y - 0.5f, z + 0.5f,   0.0f, 0.0f, 1.0f,   (float)type,        0.0f, 1.0f,
                    });
                }
                if ((z == 0) || (blocks[x][y][z-1] == 0)) {
                    mesh.insert(mesh.end(), {
                        x - 0.5f, y - 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   (float)type,        0.0f, 1.0f,
                        x + 0.5f, y + 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   (float)type,        1.0f, 1.0f,
                        x + 0.5f, y - 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   (float)type,        1.0f, 0.0f,
                        x + 0.5f, y + 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   (float)type,        1.0f, 0.0f,
                        x - 0.5f, y - 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   (float)type,        0.0f, 0.0f,
                        x - 0.5f, y + 0.5f, z - 0.5f,   0.0f, 0.0f,-1.0f,   (float)type,        0.0f, 1.0f,
                    });
                }
            }
        }
    }
    if (!mesh.empty()) {
        count = mesh.size() / 9;
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(float), mesh.data(), GL_STATIC_DRAW);
    } else {
        count = 0;
    }
}   

void Chunk::draw() {
    if (count == 0) return;
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, count);
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
            for (int y = CHUNK_HEIGHT; y >= 0; y--) {
                if (blocks[x][y][z] != 0) {
                    heightMap[x][z] = y;
                    break;
                }
            }
        }
    }
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