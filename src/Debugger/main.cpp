#include "main.h"

// Essa função teve fortes influências de IA
// mesmo assim foi desafiador implementar a lógica de visualização de chunks igual do Minecraft
Debugger::Debugger() : gridShader("shaders/grid.vert", "shaders/grid.frag") {
    lastTime = std::chrono::high_resolution_clock::now();
    frameCount = 0;

    std::vector<glm::vec3> borderVertices;
    const float offset = -0.5f;
    const float w = CHUNK_WIDTH;
    const float h = CHUNK_HEIGHT;
    const float d = CHUNK_DEPTH;

    glm::vec3 v000 = {0 + offset, 0, 0 + offset};
    glm::vec3 vW00 = {w + offset, 0, 0 + offset};
    glm::vec3 v0H0 = {0 + offset, h, 0 + offset};
    glm::vec3 vWH0 = {w + offset, h, 0 + offset};
    glm::vec3 v00D = {0 + offset, 0, d + offset};
    glm::vec3 vW0D = {w + offset, 0, d + offset};
    glm::vec3 v0HD = {0 + offset, h, d + offset};
    glm::vec3 vWHD = {w + offset, h, d + offset};

    borderVertices.push_back(v000); borderVertices.push_back(v0H0);
    borderVertices.push_back(vW00); borderVertices.push_back(vWH0);
    borderVertices.push_back(vW0D); borderVertices.push_back(vWHD);
    borderVertices.push_back(v00D); borderVertices.push_back(v0HD);
    
    borderVertices.push_back(v000); borderVertices.push_back(vW00);
    borderVertices.push_back(vW00); borderVertices.push_back(vW0D);
    borderVertices.push_back(vW0D); borderVertices.push_back(v00D);
    borderVertices.push_back(v00D); borderVertices.push_back(v000);

    borderVertices.push_back(v0H0); borderVertices.push_back(vWH0);
    borderVertices.push_back(vWH0); borderVertices.push_back(vWHD);
    borderVertices.push_back(vWHD); borderVertices.push_back(v0HD);
    borderVertices.push_back(v0HD); borderVertices.push_back(v0H0);

    this->borderVertexCount = borderVertices.size();
    
    glGenVertexArrays(1, &borderVAO);
    glGenBuffers(1, &borderVBO);
    glBindVertexArray(borderVAO);

    glBindBuffer(GL_ARRAY_BUFFER, borderVBO);
    glBufferData(GL_ARRAY_BUFFER, borderVertices.size() * sizeof(glm::vec3), borderVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    std::vector<glm::vec3> internalGridVertices;

    for (int x = 1; x < CHUNK_WIDTH; ++x) {
        internalGridVertices.push_back(glm::vec3(x + offset, 0, 0 + offset));
        internalGridVertices.push_back(glm::vec3(x + offset, h, 0 + offset));

        internalGridVertices.push_back(glm::vec3(x + offset, 0, d + offset));
        internalGridVertices.push_back(glm::vec3(x + offset, h, d + offset));
    }

    for (int z = 1; z < CHUNK_DEPTH; ++z) {
        internalGridVertices.push_back(glm::vec3(0 + offset, 0, z + offset));
        internalGridVertices.push_back(glm::vec3(0 + offset, h, z + offset));

        internalGridVertices.push_back(glm::vec3(w + offset, 0, z + offset));
        internalGridVertices.push_back(glm::vec3(w + offset, h, z + offset));
    }

    for (int y = 1; y < CHUNK_HEIGHT; ++y) {
        // Frente e trás
        internalGridVertices.push_back(glm::vec3(0 + offset, y, 0 + offset));
        internalGridVertices.push_back(glm::vec3(w + offset, y, 0 + offset));

        internalGridVertices.push_back(glm::vec3(0 + offset, y, d + offset));
        internalGridVertices.push_back(glm::vec3(w + offset, y, d + offset));

        // Laterais
        internalGridVertices.push_back(glm::vec3(0 + offset, y, 0 + offset));
        internalGridVertices.push_back(glm::vec3(0 + offset, y, d + offset));

        internalGridVertices.push_back(glm::vec3(w + offset, y, 0 + offset));
        internalGridVertices.push_back(glm::vec3(w + offset, y, d + offset));
    }

    this->internalGridVertexCount = internalGridVertices.size();

    glGenVertexArrays(1, &internalGridVAO);
    glGenBuffers(1, &internalGridVBO);
    glBindVertexArray(internalGridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, internalGridVBO);
    glBufferData(GL_ARRAY_BUFFER, internalGridVertices.size() * sizeof(glm::vec3), internalGridVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Debugger::updateBlockInfo(const World &world, const Vec3i pos) {
    std::stringstream ss1, ss2;
    ss1.precision(2);
    ss1.precision(2);
    if (pos.x == 999 && pos.y == 999 && pos.z == 999) {
        ss1 << "Posição atual do bloco: [N/A]";
        blockPosInfo = ss1.str();
    } else {
        ss1 << "Posição atual do bloco: [" << pos.x << "," << pos.y << "," << pos.z << "]";
        blockPosInfo = ss1.str();
    }
    int blockType = world.getBlockType(pos);
    if (blockType == -1) {
        ss2 << "Tipo atual do bloco: [N/A]";
        blockTypeInfo = ss2.str();
    } else {
        ss2 << "Tipo atual do bloco: [" << blockType << "]";
        blockTypeInfo = ss2.str();
    }
}

void Debugger::updateCamInfo(const Camera &camera) {
    glm::vec3 pos = camera.getPos();
    std::stringstream ss;
    ss.precision(4);
    ss << "Posição atual da camêra: [" << pos.x << "," << pos.y << "," << pos.z << "]";
    camInfo = ss.str();
}

void Debugger::updateChunkInfo(const Chunk &chunk) {
    std::stringstream ss;
    ss.precision(2);
    if (&chunk != nullptr) {
        ss << "Quantidade de malhas da chunk atual: " << chunk.getCount();
        chunkInfo = ss.str();
    } else {
        ss << "Quantidade de malhas da chunk atual: 0";
        chunkInfo = ss.str();
    }
}
// talvez inútil pois ImGui já oferece uma função sofisticada para FPS
void Debugger::updateFPS() {
    frameCount++;
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedTime = currentTime - lastTime;
    if (elapsedTime.count() >= 0.1) {
        double fps = static_cast<double>(frameCount)/ elapsedTime.count();
        std::stringstream ss;
        ss.precision(2);
        ss << "FPS: " << fps;
        fpsInfo = ss.str();
        frameCount = 0;
        lastTime = currentTime;
    }
}

void Debugger::updateSeed(const int seed) {
    std::stringstream ss;
    ss.precision(2);
    ss << "Semente atual do mundo: " << seed;
    seedInfo = ss.str();
}

void Debugger::drawChunkGrid(const Camera& camera, const glm::mat4 &proj, const glm::mat4 &view) {
    glm::vec3 camPos = camera.getPos();
    int chunkX = floor(camPos.x / CHUNK_WIDTH);
    int chunkY = 0;
    int chunkZ = floor(camPos.z / CHUNK_DEPTH);
    gridShader.use();
    gridShader.setMat4("projection", proj);
    gridShader.setMat4("view", view);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(chunkX * CHUNK_WIDTH, chunkY * CHUNK_HEIGHT, chunkZ * CHUNK_DEPTH));
    gridShader.setMat4("model", model);

    glLineWidth(2.0f);
    gridShader.setVec3("uColor", 0.2f, 0.2f, 1.0f); // azul
    glBindVertexArray(borderVAO);
    glDrawArrays(GL_LINES, 0, borderVertexCount);

    glLineWidth(1.0f);
    gridShader.setVec3("uColor", 1.0f, 1.0f, 0.0f); // amarelo
    glBindVertexArray(internalGridVAO);
    glDrawArrays(GL_LINES, 0, internalGridVertexCount);
    glBindVertexArray(0);
}


void Debugger::check() {
    return;
}