#include "world.h"

World::World() {
    // 1. DEFINIÇÃO DAS VARIÁVEIS
    srand(time(NULL));
    this->seed = 1001 * rand();
    noise.SetSeed(seed);
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    saveDir = "worlds/sandbox_world";
    if (std::filesystem::exists(saveDir)) {
        try {
            std::filesystem::remove_all(saveDir);
            std::cout << "Arquivos do mundo anterior limpo." << std::endl;
        } catch (const std::filesystem::filesystem_error &e) {
            std::cout << "Erro ao remover arquivos do mundo anterior" << e.what() << std::endl;
        }
    }
    std::filesystem::create_directories(saveDir);

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
    return;
}

Chunk* World::getChunk(int x, int y, int z) {
    auto it = world.find({x, y, z});
    if (it != world.end()) {
        return &it->second;
    }
    return nullptr;
}

const Chunk* World::getChunk(int x, int y, int z) const{
    auto it = world.find({x, y, z});
    if (it != world.end()) {
        return &it->second;
    }
    return nullptr;
}

const std::map<Vec3i, Chunk> World::getChunks() const{
    return world;
}

int World::getBlockType(const Vec3i &pos) const {
    int chunkX = static_cast<int>(floor((float)pos.x/CHUNK_WIDTH));
    int chunkY = static_cast<int>(floor((float)pos.y/CHUNK_HEIGHT));
    int chunkZ = static_cast<int>(floor((float)pos.z/CHUNK_DEPTH));
    Vec3i chunkPos = { chunkX, chunkY, chunkZ };
    const Chunk* chunk = getChunk(chunkPos.x, chunkPos.y, chunkPos.z);
    if (chunk) {
        int localX = pos.x - chunkPos.x * CHUNK_WIDTH;
        int localY = pos.y - chunkPos.y * CHUNK_HEIGHT;
        int localZ = pos.z - chunkPos.z * CHUNK_DEPTH;
        return chunk->getBlock(localX, localY, localZ);
    }
    return -1;
}

int World::getSeed() const {
    return seed;
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
        chunk->setBlock(localX, localY, localZ, 1, true);
        chunk->updateHeightMap();
        chunk->buildMesh((*this), chunkPos);
        chunk->buildWaterMesh((*this), chunkPos);
        for (int dx = -1; dx <= 1; dx++) {
            for (int dz = -1; dz <= 1; dz++) {
                Vec3i neighborPos = {chunkPos.x + dx, chunkPos.y, chunkPos.z + dz};
                if (getChunk(neighborPos.x, neighborPos.y, neighborPos.z)) {
                    chunksNeedingLighting.insert(neighborPos);
                }
            }
        }
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
        chunk->setBlock(localX, localY, localZ, 0, true);
        chunk->updateHeightMap();
        chunk->buildMesh((*this), chunkPos);
        chunk->buildWaterMesh((*this), chunkPos);
        for (int dx = -1; dx <= 1; dx++) {
            for (int dz = -1; dz <= 1; dz++) {
                Vec3i neighborPos = {chunkPos.x + dx, chunkPos.y, chunkPos.z + dz};
                if (getChunk(neighborPos.x, neighborPos.y, neighborPos.z)) {
                    chunksNeedingLighting.insert(neighborPos);
                }
            }
        }
    }
}

void World::update(const glm::vec3 &pos) {
    int renderDistance = 5; // Isso cria uma área de (5*2+1) x (5*2+1) = 11x11 chunks

    // Encontra em qual chunk o jogador está
    int playerChunkX = static_cast<int>(floor(pos.x / CHUNK_WIDTH));
    int playerChunkZ = static_cast<int>(floor(pos.z / CHUNK_DEPTH));

    // FASE 1: Carregamento/Geração de chunks
    std::vector<Vec3i> newlyCreatedChunks;
    
    for (int i = playerChunkX - renderDistance; i <= playerChunkX + renderDistance; i++) {
        for (int j = playerChunkZ - renderDistance; j <= playerChunkZ + renderDistance; j++) {
            Vec3i chunkPos = {i, 0, j};

            // Verifica se o chunk já existe no nosso mapa
            if (world.find(chunkPos) == world.end()) {
                std::string filename = "chunk_" + std::to_string(i) + "_" + std::to_string(j) + ".chunk";
                std::filesystem::path filepath = saveDir/filename;
                Chunk newChunk;
                if (!newChunk.loadFile(filepath.string(), (*this), chunkPos)) {
                    generateChunkData(newChunk, chunkPos);
                    newlyCreatedChunks.push_back(chunkPos); // Marca como recém-criado
                }
                world[chunkPos] = std::move(newChunk);
            }
        }
    }

    // FASE 2: Atualização de iluminação para chunks recém-criados
    for (const auto& chunkPos : newlyCreatedChunks) {
        chunksNeedingLighting.insert(chunkPos);
    }
    
    // Atualiza iluminação dos chunks pendentes
    updateChunkLighting();

    // FASE 3: Descarregamento de chunks distantes
    std::vector<Vec3i> chunksToUnload;

    for (auto const& [pos, chunk] : world) {
        int dist_x = abs(pos.x - playerChunkX);
        int dist_z = abs(pos.z - playerChunkZ);

        if (dist_x > renderDistance || dist_z > renderDistance) {
            chunksToUnload.push_back(pos);
        }
    }

    for (const auto& pos : chunksToUnload) {
        Chunk &chunk = world.at(pos);
        if (chunk.isModified()) {
            std::string filename = "chunk_" + std::to_string(pos.x) + "_" + std::to_string(pos.z) + ".chunk";
            std::filesystem::path filepath = saveDir/filename;
            chunk.saveFile(filepath.string());
        }
        // Remove da lista de chunks pendentes de iluminação também
        chunksNeedingLighting.erase(pos);
        world.erase(pos);
    }
}

void World::updateChunkLighting() {
    std::vector<Vec3i> completedChunks;
    
    for (const auto& chunkPos : chunksNeedingLighting) {
        auto it = world.find(chunkPos);
        if (it != world.end()) {
            // Verifica se os chunks vizinhos necessários existem
            bool canCalculateLighting = true;
            
            // Para um cálculo básico de skylight, precisamos principalmente do próprio chunk
            // Mas para iluminação mais avançada, você pode verificar vizinhos aqui
            
            if (canCalculateLighting) {
                Chunk& chunk = it->second;
                chunk.updateHeightMap();
                chunk.buildMesh((*this), chunkPos);
                chunk.buildWaterMesh((*this), chunkPos);
                completedChunks.push_back(chunkPos);
            }
        }
    }
    
    // Remove chunks que tiveram a iluminação calculada
    for (const auto& pos : completedChunks) {
        chunksNeedingLighting.erase(pos);
    }
}

void World::generateChunkData(Chunk &chunk, Vec3i chunkPos) {
    // 2. PREENCHIMENTO DOS CHUNKS
    for (int bx = 0; bx < CHUNK_WIDTH; bx++) {
        for (int bz = 0; bz < CHUNK_DEPTH; bz++) {

            float worldX = (chunkPos.x * CHUNK_WIDTH) + bx;
            float worldZ = (chunkPos.z * CHUNK_DEPTH) + bz;
            int SEA_HEIGHT = CHUNK_HEIGHT / 4;
            noise.SetFrequency(0.005f); // 0.0005 planice <=> 0.05 montanha
            float terrainNoise = noise.GetNoise(worldX, worldZ);
            int terrainHeight = (int)(CHUNK_HEIGHT/2) * (1.0f + terrainNoise);
            noise.SetFrequency(0.01f);
            float waterNoise = noise.GetNoise(worldX, worldZ);
            int waterHeight = (int)(CHUNK_HEIGHT*0.3f);

            // ÁGUA - Nível base global
            int baseSeaLevel = CHUNK_HEIGHT / 3; // Nível fixo global
            
            // Lagos elevados apenas em áreas muito específicas
            noise.SetFrequency(0.005f); // Frequência mais baixa = lagos maiores
            float lakeNoise = noise.GetNoise(worldX, worldZ);
            
            int waterLevel = baseSeaLevel;
            
            // Apenas lagos muito grandes e suaves
            if (lakeNoise > 0.6f) { // Threshold alto = menos lagos
                int lakeHeight = (int)(10 * (lakeNoise - 0.6f)); // Lagos até 4 blocos mais altos
                waterLevel = baseSeaLevel + lakeHeight;
            }
            
            // Garantir que não vai muito alto
            waterLevel = std::min(waterLevel, CHUNK_HEIGHT - 20);
            
            // Garantir limites mínimos
            waterHeight = std::max(5, std::min(waterHeight, CHUNK_HEIGHT - 10));

            for (int by = 0; by <  CHUNK_HEIGHT; by++) {
                if (by == 0) {
                    chunk.setBlock(bx, by, bz, 4, false); // Rocha matriz
                } else {
                    float worldY = (float)by;
                    noise.SetFrequency(0.05);
                    float caveNoise = noise.GetNoise(worldX, worldY, worldZ);
                    if (caveNoise > 0.1f && by <= (int)terrainHeight*0.8) {
                        if (by <=  baseSeaLevel * 0.7f) {
                            chunk.setBlock(bx, by, bz, 5, false); // Água    
                        } else {
                            chunk.setBlock(bx, by, bz, 0, false); // Ar         
                        }
                    } else if (by < terrainHeight) {
                            // Grama
                            if (terrainHeight - by < 3) { chunk.setBlock(bx, by, bz, 1, false); } 
                            // Terra
                            else if (terrainHeight - by >= 3 && terrainHeight - by < 6) { chunk.setBlock(bx, by, bz, 2, false); } 
                            // Pedra
                            else { chunk.setBlock(bx, by, bz, 3, false); }
                        } else {
                            //if (by <= waterLevel) {
                            //    chunk.setBlock(bx, by, bz, 5, false); // Água
                            //} else {
                                chunk.setBlock(bx, by, bz, 0, false);
                            //}
                        }
                    }
                }
            }
        }
    // 3. CONSTRUÇÃO DA MALHA OTIMIZADA
    chunk.updateHeightMap();
}

void World::draw(Shader &shader, const glm::mat4 &projection, const glm::mat4 &view, const glm::vec3 &cameraPos) {
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);
    
    int renderDistance = 5;
    int playerChunkX = static_cast<int>(floor(cameraPos.x / CHUNK_WIDTH));
    int playerChunkZ = static_cast<int>(floor(cameraPos.z / CHUNK_DEPTH));

    glDepthMask(GL_TRUE);
    for (int i = playerChunkX - renderDistance; i <= playerChunkX + renderDistance; i++) {
        for (int j = playerChunkZ - renderDistance; j <= playerChunkZ + renderDistance; j++) {
            Vec3i pos = {i, 0, j};
            
            // Tenta encontrar o chunk no mapa
            auto it = world.find(pos);
            if (it != world.end()) {
                // Se o chunk existe, desenha ele
                Chunk& chunk = it->second;
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(pos.x * CHUNK_WIDTH, pos.y * CHUNK_HEIGHT, pos.z * CHUNK_DEPTH));
                shader.setMat4("model", model);
                chunk.draw();
            }
        }
    }

    glDepthMask(GL_FALSE);
    for (int i = playerChunkX - renderDistance; i <= playerChunkX + renderDistance; i++) {
        for (int j = playerChunkZ - renderDistance; j <= playerChunkZ + renderDistance; j++) {
            Vec3i pos = {i, 0, j};
            
            // Tenta encontrar o chunk no mapa
            auto it = world.find(pos);
            if (it != world.end()) {
                // Se o chunk existe, desenha ele
                Chunk& chunk = it->second;
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(pos.x * CHUNK_WIDTH, pos.y * CHUNK_HEIGHT, pos.z * CHUNK_DEPTH));
                shader.setMat4("model", model);
                chunk.drawWater();
            }
        }
    }

    glDepthMask(GL_TRUE);
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

bool World::isExposedToSky(int x, int y, int z) const {
    int chunkX = static_cast<int>(floor((float)x/CHUNK_WIDTH));
    int chunkY = static_cast<int>(floor((float)y/CHUNK_HEIGHT));
    int chunkZ = static_cast<int>(floor((float)z/CHUNK_DEPTH));
    Vec3i chunkPos = { chunkX, chunkY, chunkZ };
    auto it = world.find(chunkPos);
    if (it == world.end()) {
        return false;
    }
    int localX = x - chunkPos.x * CHUNK_WIDTH;
    int localY = y - chunkPos.y * CHUNK_HEIGHT;
    int localZ = z - chunkPos.z * CHUNK_DEPTH;
    
    int heightValue = it->second.getHeightValue(localX, localZ);
    return heightValue == -1 || heightValue <= localY;
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