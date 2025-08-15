#include "main.h"
#include "../Shaders/main.h"
#include "../Other/FastNoiseLite.h"

World::World(int range) {
    // 1. DEFINIÇÃO DAS VARIÁVEIS
    this->range = range;

    // 2. CRIANDO OS OBJETOS PARA VERTICES E ARESTAS
    float vData[] = {
        // Posições             // Normais
        // Face de Trás
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,

        // Face da Frente
        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,

        // Face da Esquerda
        -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,

        // Face da Direita
        0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,

        // Face de Baixo
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,

        // Face de Cima
        -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f
    };

    float bData[] = {
        // Frente
        -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,

        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,

        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        // Trás
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,

        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,

        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        // Conexões frente-trás
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        0.5f, -0.5f,  0.5f,
        0.5f, -0.5f, -0.5f,

        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

    int stride = 6 * sizeof(float);

    // 3. CRIANDO VAO E VBO (MUNDO)
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vData), vData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 4. CRIANDO VAO E VBO (BORDAS)
    glGenVertexArrays(1, &border_VAO);
    glGenBuffers(1, &border_VBO);
    glBindVertexArray(border_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, border_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bData), bData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    check();
}

void World::create() {
    FastNoiseLite noise;
    srand(time(NULL));
    int seed = 1001 * rand();
    // vai virar função da classe Debugger
    std::cout << "Gerando mundo com a semente: " << seed << std::endl;
    noise.SetSeed(seed);
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    int yRange = 1;
    for (int i = -range; i < range; i++) {
        for (int j = -range; j < range; j++) {
                // 1. CRIAÇÃO DOS CHUNKS
                Vec3i chunkPos = {i, 0, j};
                world[chunkPos] = Chunk();

                // 2. PREENCHIMENTO DOS CHUNKS
                for (int bx = 0; bx < CHUNK_WIDTH; bx++) {
                    for (int bz = 0; bz < CHUNK_DEPTH; bz++) {

                        float worldX = (i * CHUNK_WIDTH) + bx;
                        float worldZ = (j * CHUNK_DEPTH) + bz;
                        noise.SetFrequency(0.025f); // valores menores -> montanhas mais altas
                        float terrainNoise = noise.GetNoise(worldX, worldZ);
                        int terrainHeight = (int)(CHUNK_HEIGHT/2) * (1.0f + terrainNoise);
                        //int terrainHeight = 40 + (int)(30.0f * (terrainNoise + 1.0f) * 0.5f);

                        for (int by = 0; by <  CHUNK_HEIGHT; by++) {
                            if (by == 0) {
                                world.at(chunkPos).setBlock(bx, by, bz, 4);
                            }
                            else if (by < terrainHeight) {
                                float worldY = (float)by;
                                noise.SetFrequency(0.1f);
                                float caveNoise = noise.GetNoise(worldX, worldY, worldZ);
                                if (caveNoise < 0.4f) {
                                    if (terrainHeight - by < 3) {
                                        world.at(chunkPos).setBlock(bx, by, bz, 1); // Grama
                                    } else if (terrainHeight - by >= 3 && terrainHeight - by < 6) {
                                        world.at(chunkPos).setBlock(bx, by, bz, 2); // Terra
                                    } else {
                                        world.at(chunkPos).setBlock(bx, by, bz, 3); // Pedra
                                    }
                                } else {
                                world.at(chunkPos).setBlock(bx, by, bz, 0); // Ar
                            }
                        }
                    }
                }
            }

            // 3. CONSTRUÇÃO DA MALHA OTIMIZADA
            world.at(chunkPos).buildMesh();
        }
    }
}

Chunk* World::getChunk(int x, int y, int z) {
    auto it = world.find({x, y, z});
    if (it != world.end()) {
        return &it->second;
    }
    return nullptr;
}

void World::addBlock(const Vec3i &pos) {
    int chunkX = static_cast<int>(floor((float)pos.x/CHUNK_WIDTH));
    int chunkY = static_cast<int>(floor((float)pos.y/CHUNK_HEIGHT));
    int chunkZ = static_cast<int>(floor((float)pos.z/CHUNK_DEPTH));
    Vec3i chunkPos = { chunkX, chunkY, chunkZ };
    Chunk* chunk = getChunk(chunkPos.x, chunkPos.y, chunkPos.z);
    if (chunk) {
        int localX = pos.x - chunkPos.x * CHUNK_WIDTH;
        int localY = pos.y - chunkPos.y * CHUNK_HEIGHT;
        int localZ = pos.z - chunkPos.z * CHUNK_DEPTH;
        chunk->setBlock(localX, localY, localZ, 1);
        chunk->buildMesh();
    }
}

void World::removeBlock(const Vec3i &pos) {
    int chunkX = static_cast<int>(floor((float)pos.x/CHUNK_WIDTH));
    int chunkY = static_cast<int>(floor((float)pos.y/CHUNK_HEIGHT));
    int chunkZ = static_cast<int>(floor((float)pos.z/CHUNK_DEPTH));
    Vec3i chunkPos = { chunkX, chunkY, chunkZ };
    Chunk* chunk = getChunk(chunkPos.x, chunkPos.y, chunkPos.z);
    if (chunk) {
        int localX = pos.x - chunkPos.x * CHUNK_WIDTH;
        int localY = pos.y - chunkPos.y * CHUNK_HEIGHT;
        int localZ = pos.z - chunkPos.z * CHUNK_DEPTH;
        chunk->setBlock(localX, localY, localZ, 0);
        chunk->buildMesh();
    }
}

void World::draw(Shader &shader, const glm::mat4 &projection, const glm::mat4 &view, const glm::vec3 &cameraPos) {
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);
    for (auto const& [pos, chunk]: world) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(pos.x * CHUNK_WIDTH, pos.y * CHUNK_HEIGHT, pos.z * CHUNK_DEPTH));
        shader.setMat4("model", model);
        const_cast<Chunk&>(chunk).draw();
    }
}

void World::highlight(Shader &shader, const Vec3i &pos, const glm::mat4 &projection, const glm::mat4 &view) {
    if (!hasBlockAt(pos)) return;
    shader.use();
    glBindVertexArray(border_VAO);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos.x, pos.y, pos.z));
    glm::mat4 transform = projection * view * model;
    //unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
    //glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
    shader.setMat4("transform", transform);
    glDrawArrays(GL_LINES, 0, 24);
    glBindVertexArray(0);
}

bool World::hasBlockAt(const Vec3i &pos) const {
    int chunkX = static_cast<int>(floor((float)pos.x/CHUNK_WIDTH));
    int chunkY = static_cast<int>(floor((float)pos.y/CHUNK_HEIGHT));
    int chunkZ = static_cast<int>(floor((float)pos.z/CHUNK_DEPTH));
    Vec3i chunkPos = { chunkX, chunkY, chunkZ };
    auto it = world.find(chunkPos);
    if (it == world.end()) {
        return false;
    }
    int localX = pos.x - chunkPos.x * CHUNK_WIDTH;
    int localY = pos.y - chunkPos.y * CHUNK_HEIGHT;
    int localZ = pos.z - chunkPos.z * CHUNK_DEPTH;
    return it->second.getBlock(localX, localY, localZ) != 0;
}

void World::check() {
    return;
}