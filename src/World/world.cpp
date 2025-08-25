#include "world.h"

std::vector<Biome> biomes = {
    {
        "Planície", ID_GRAMA, ID_TERRA, 
        32.0f, 1.0f, 0.01f, 
        0.2f, 0.3f,
        true,
    },
    {
        "Planície nevada", ID_GRAMA_NEVE, ID_TERRA,
        32.0f, 1.0f, 0.01f,
        -0.5f, 0.0f,
        false,
    },
    {
        "Montanhas", ID_PEDRA, ID_PEDRA, 
        60.0f, 30.0f, 0.03f, 
        0.4f, 0.0f,
        false,
    },
    {
        "Deserto", ID_AREIA, ID_ARENITO, 
        34.0f, 3.0f, 0.01f, 
        0.8f, -0.7f,
        false,
    },
    {
        "Praia", ID_AREIA, ID_ARENITO,
        24.0f, 2.0f, 0.01f,
        0.8f, 0.5f,
        true,
    },
    {
        "Vales", ID_GRAMA, ID_TERRA,
        20.0f, 50.0f, 0.04f,
        0.4f, 0.0f,
        false,
    },
};

std::map<int, Block> blockDatabase;

void initializeBlockDatabase() {
    // Formato: {nome, tipo, {col, lin}_topo, {col, lin}_lado, {col, lin}_fundo}

    blockDatabase[ID_AR] = {"Ar", ID_AR, {-1, -1}, {-1, -1}, {-1, -1}};
    
    // Bloco de Grama: topo verde, lado com terra, fundo de terra
    blockDatabase[ID_GRAMA] = {"Grama", ID_GRAMA, {63, 6}, {60, 6}, {27, 5}};
    
    // Bloco de Grama (Neve):
    blockDatabase[ID_GRAMA_NEVE] = {"Neve", ID_GRAMA_NEVE, {33, 11}, {62, 6}, {27, 5}};
    
    // Bloco de Terra: mesma textura em todos os lados
    blockDatabase[ID_TERRA] = {"Terra", ID_TERRA, {27, 5}, {27, 5}, {27, 5}};

    // Bloco de Pedra: mesma textura em todos os lados
    blockDatabase[ID_PEDRA] = {"Pedra", ID_PEDRA, {13, 14}, {13, 14}, {13, 14}};

    // Rocha Matriz (Bedrock): uma pedra mais escura e resistente
    blockDatabase[ID_ROCHA_MATRIZ] = {"Rocha matriz", ID_ROCHA_MATRIZ, {48, 0}, {48, 0}, {48, 0}};
    
    // Água: uma textura de água animada ou estática
    blockDatabase[ID_AGUA] = {"Água", ID_AGUA, {12, 16}, {13, 16}, {13, 16}};

    // Areia: para praias e desertos
    blockDatabase[ID_AREIA] = {"Areia", ID_AREIA, {53, 12}, {53, 12}, {53, 12}};
    
    // Arenito: bloco subsuperfície do deserto
    blockDatabase[ID_ARENITO] = {"Arenito", ID_ARENITO, {56, 12}, {54, 12}, {55, 12}};


}

World::World() {
    initializeBlockDatabase();
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
    Block blockDictionary[] = {{"Ar", 0}, {"Grama", 1}, {"Terra", 2}, {"Pedra", 3}, {"Rocha matriz", 4}, {"Água", 5}};
    for (auto it: blockDictionary) {
        blockSummary[it.type] = std::move(it.name);
    }

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

const std::unordered_map<Vec3i, Chunk, Vec3iHasher> World::getChunks() const{
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

std::string World::getBlockName(int type) const {
    if (type >= 0 && type < blockDatabase.size()) {
        return blockDatabase[type].name;
    }
    return blockDatabase[ID_AR].name;
}

Block World::getBlockInfo(int type) const {
    if (type >= 0 && type < blockDatabase.size()) {
        return blockDatabase[type];
    }
    return blockDatabase[ID_AR];
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

void World::update(const glm::vec3 &pos, const glm::mat4 &view, const glm::mat4 &proj) {
    int renderDistance = 5;

    int playerChunkX = static_cast<int>(floor(pos.x / CHUNK_WIDTH));
    int playerChunkZ = static_cast<int>(floor(pos.z / CHUNK_DEPTH));

    // FASE 1: Carregamento/Geração de chunks
    std::vector<Vec3i> newlyLoadedChunks;

    Frustum frustum;
    frustum.update(view, proj);
    
    for (int i = playerChunkX - renderDistance; i <= playerChunkX + renderDistance; i++) {
        for (int j = playerChunkZ - renderDistance; j <= playerChunkZ + renderDistance; j++) {
            Vec3i chunkPos = {i, 0, j};

            glm::vec3 minPos = {chunkPos.x * CHUNK_WIDTH, chunkPos.y * CHUNK_HEIGHT, chunkPos.z * CHUNK_DEPTH};
            glm::vec3 maxPos = {(chunkPos.x * CHUNK_WIDTH) + CHUNK_WIDTH, (chunkPos.y * CHUNK_HEIGHT) + CHUNK_HEIGHT, (chunkPos.z * CHUNK_DEPTH) + CHUNK_DEPTH};
            AABB chunkAABB = {minPos, maxPos};
            if (frustum.isBoxInFrustum(chunkAABB)) {
                if (world.find(chunkPos) == world.end()) {
                    std::string filename = "chunk_" + std::to_string(i) + "_" + std::to_string(j) + ".chunk";
                    std::filesystem::path filepath = saveDir/filename;
                    Chunk newChunk;
                    if (!newChunk.loadFile(filepath.string(), (*this), chunkPos)) {
                        //Profiler::measure("World::generateChunkData()", [&]() {
                            generateChunkData(newChunk, chunkPos);
                        //});
                        newlyLoadedChunks.push_back(chunkPos);
                    }
                    world[chunkPos] = std::move(newChunk);
                    newlyLoadedChunks.push_back(chunkPos);
                }
            }
        }
    }

    // FASE 2: Colocar os chunks novos e seus vizinhos na fila para construir a malha
    for (const auto& chunkPos : newlyLoadedChunks) {
        for (int dx = -1; dx <= 1; dx++) {
            for (int dz = -1; dz <= 1; dz++) {
                Vec3i posToUpdate = {chunkPos.x + dx, 0, chunkPos.z + dz};
                if (world.count(posToUpdate)) {
                    chunksNeedingLighting.insert(posToUpdate);
                }
            }
        }
    }
    
    // Atualiza iluminação dos chunks pendentes
    //Profiler::measure("World::updateChunkLighing()", [&]() {
        updateChunkLighting();
    //});

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
        chunksNeedingLighting.erase(pos);
        world.erase(pos);
    }
}

void World::updateChunkLighting() {
    std::vector<Vec3i> completedChunks;
    int chunksBuiltThisFrame = 0;
    int MAX_CHUNKS_PER_FRAME = 2;
    
    for (const auto& chunkPos : chunksNeedingLighting) {
        if (chunksBuiltThisFrame == MAX_CHUNKS_PER_FRAME) break;
        auto it = world.find(chunkPos);
        if (it != world.end()) {
            bool canCalculateLighting = true;
            if (canCalculateLighting) {
                Chunk& chunk = it->second;
                chunk.updateHeightMap();
                chunk.buildMesh((*this), chunkPos);
                chunk.buildWaterMesh((*this), chunkPos);
                completedChunks.push_back(chunkPos);
                chunksBuiltThisFrame++;
            }
        }
    }
    
    // Remove chunks que tiveram a iluminação calculada
    for (const auto& pos : completedChunks) {
        chunksNeedingLighting.erase(pos);
    }
}

void World::generateChunkData(Chunk &chunk, Vec3i chunkPos) {

    FastNoiseLite temperatureNoise, humidityNoise, waterNoise, lakeNoise, caveNoise;
    temperatureNoise.SetSeed(seed+1);
    temperatureNoise.SetFrequency(0.0005f);
    humidityNoise.SetSeed(seed+2);
    humidityNoise.SetFrequency(0.0005f);
    waterNoise.SetSeed(seed+3);
    waterNoise.SetFrequency(0.01f);
    lakeNoise.SetSeed(seed+4);
    lakeNoise.SetFrequency(0.005f);
    caveNoise.SetSeed(seed+5);
    caveNoise.SetFrequency(0.05f);

    // --- FASE 1: Gerar o terreno inicial ---
    for (int bx = 0; bx < CHUNK_WIDTH; bx++) {
        for (int bz = 0; bz < CHUNK_DEPTH; bz++) {

            float worldX = (chunkPos.x * CHUNK_WIDTH) + bx;
            float worldZ = (chunkPos.z * CHUNK_DEPTH) + bz;

            float tempValue = temperatureNoise.GetNoise(worldX, worldZ);
            float humidityValue = humidityNoise.GetNoise(worldX, worldZ);

            std::vector<float> weights;
            float totalWeight = 0.0f;
            
            // Cálculo do peso de cada bioma, somando com o peso total dos biomas
            for (const auto &biome: biomes) {
                float distTemp = tempValue - biome.idealTemp;
                float distHumidity = humidityValue - biome.idealHumidity;
                float distance = sqrt(distTemp * distTemp + distHumidity * distHumidity);
                float weight = 1.0f / (pow(distance, 2.0f) + 0.0001f);
                weights.push_back(weight);
                totalWeight += weight;
            }

            // Normalização dos pesos de cada bioma (dessa forma cada peso varia entre [0, 1])
            for (int i = 0; i < biomes.size(); i++) {
                weights[i] /= totalWeight;
            }

            if (bx == 0 && bz == 0) {
                std::cout << "--- Chunk [" << chunkPos.x << ", " << chunkPos.z << "] ---" << std::endl;
                std::cout << "Valores de Ruido: Temp=" << tempValue << ", Umid=" << humidityValue << std::endl;
                for (int i = 0; i < biomes.size(); i++) {
                    std::cout << "  - Bioma: " << biomes[i].name << ", Peso: " << weights[i] << std::endl;
                }
            }

            float finalHeight = 0.0f;

            // Calcula a altura com base na média ponderada das alturas de cada bioma
            for (int i = 0; i < biomes.size(); i++) {
                const Biome &biome = biomes[i];
                noise.SetFrequency(biome.frequency);
                float terrainNoise = noise.GetNoise(worldX, worldZ);
                float biomeHeight = biome.baseHeight + (biome.amplitude * terrainNoise);
                finalHeight += biomeHeight * weights[i];
            }

            
            int domainBiomeIndex = std::distance(weights.begin(), std::max_element(weights.begin(), weights.end()));
            const Biome &currentBiome = biomes[domainBiomeIndex];
            //const Biome& currentBiome = biomes[3]; // Pra depurar um terreno específico

            int terrainHeight = (int)finalHeight;

            int waterLevel = CHUNK_HEIGHT/5;

            for (int by = 0; by < CHUNK_HEIGHT; by++) {
                float worldY = (float)by;
                float caveValue = caveNoise.GetNoise(worldX, worldY, worldZ);
                    if (caveValue > 0.01f && by <= (int)(terrainHeight*0.8) && by >= (int)(terrainHeight*0.2)) {
                        chunk.setBlock(bx, by, bz, 0, false);
                    } else {
                        if (by > terrainHeight) {
                            if (by <= waterLevel) {
                               chunk.setBlock(bx, by, bz, ID_AGUA, false);
                            } else {
                                chunk.setBlock(bx, by, bz, ID_AR, false);
                            }
                        } else if (by == terrainHeight) {
                            chunk.setBlock(bx, by, bz, currentBiome.topBlock, false);
                        } else if (by < terrainHeight && by > terrainHeight-5){
                            chunk.setBlock(bx, by, bz, currentBiome.subsurfaceBlock, false);
                        } else if (by == 0) {
                            chunk.setBlock(bx, by, bz, ID_ROCHA_MATRIZ, false);
                        } else {
                            chunk.setBlock(bx, by, bz, ID_PEDRA, false);
                        }
                    }
            }
        }
    }

    // --- FASE 1.5: Atualizar mapa de altura do chunk ---
    chunk.updateHeightMap();
    
    // --- FASE 2: Adicionar Decorações/Estruturas ---
    // for (int bx = 0; bx < CHUNK_WIDTH; bx++) {
    //     for (int bz = 0; bz < CHUNK_DEPTH; bz++) {
    //         int surfaceY = chunk.getHeightValue(bx, bz);
    //         if (surfaceY == -1) continue;
    //         if (rand() % 10 == 9) {
    //             if (chunk.getBlock(bx, surfaceY, bz) == ID_GRAMA) {
    //                 chunk.setBlock(bx, surfaceY + 1, bz, ID_ABOBORA, false);
    //             }
    //         }
    //     }
    // }
}

void World::draw(Shader &shader, const glm::mat4 &projection, const glm::mat4 &view, const glm::vec3 &cameraPos) {
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);

    Frustum frustum;
    frustum.update(view, projection);
    
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
                glm::vec3 minPos = {pos.x * CHUNK_WIDTH, pos.y * CHUNK_HEIGHT, pos.z * CHUNK_DEPTH};
                glm::vec3 maxPos = {(pos.x * CHUNK_WIDTH) + CHUNK_WIDTH, (pos.y * CHUNK_HEIGHT) + CHUNK_HEIGHT, (pos.z * CHUNK_DEPTH) + CHUNK_DEPTH};
                AABB chunkAABB = {minPos, maxPos};
                if (frustum.isBoxInFrustum(chunkAABB)) {
                    Chunk& chunk = it->second;
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(pos.x * CHUNK_WIDTH, pos.y * CHUNK_HEIGHT, pos.z * CHUNK_DEPTH));
                    shader.setMat4("model", model);
                    chunk.draw();
                }
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
                glm::vec3 minPos = {pos.x * CHUNK_WIDTH, pos.y * CHUNK_HEIGHT, pos.z * CHUNK_DEPTH};
                glm::vec3 maxPos = {(pos.x * CHUNK_WIDTH) + CHUNK_WIDTH, (pos.y * CHUNK_HEIGHT) + CHUNK_HEIGHT, (pos.z * CHUNK_DEPTH) + CHUNK_DEPTH};
                AABB chunkAABB = {minPos, maxPos};
                if (frustum.isBoxInFrustum(chunkAABB)) {
                    Chunk& chunk = it->second;
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(pos.x * CHUNK_WIDTH, pos.y * CHUNK_HEIGHT, pos.z * CHUNK_DEPTH));
                    shader.setMat4("model", model);
                    chunk.drawWater();
                }
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