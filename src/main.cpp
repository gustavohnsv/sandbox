#include "Other/stb_image.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "Common/main.h"
#include "Shaders/main.h"
#include "Camera/main.h"
#include "World/main.h"
#include "Debugger/main.h"
#include "Texture/main.h"
#include "Debugger/main.h"

struct ApplicationContext {
    Camera *camera;
    World *world;
};

// ============================================================================= 
// Função de leitura de arquivos de maneira recursiva
// =============================================================================
std::optional<std::filesystem::path> findPath(const std::filesystem::path start, std::string_view filename) {
    if (!std::filesystem::exists(start) || !std::filesystem::is_directory(start)) {
        std::cout << "Erro: Diretório inicial não é um diretório ou é inexistente." << std::endl;
        return std::nullopt;
    }
    auto it = std::filesystem::recursive_directory_iterator(start);
    try {
        for (const auto &entry : it) {
            if (entry.is_regular_file() && entry.path().filename() == filename) {
                return entry.path();
            }
        }
    } catch (const std::filesystem::filesystem_error &e) {
        std::cout << "Erro: Filesystem não conseguiu acessar: " << e.path1() << ":" << e.what() << std::endl;
    }
    return std::nullopt;
}

// ============================================================================= 
// Callback para dados de entrada do mouse (movimentação)
// =============================================================================
void mouse_callback(GLFWwindow* window, double xpos, double ypos) { // pode ser mantida
    ApplicationContext *context = (ApplicationContext*) glfwGetWindowUserPointer(window);
    Camera *camera = context->camera;
    if (camera) {
        camera->processMouseMovement(xpos, ypos);
    }
}

// ============================================================================= 
// Callback para dados de entrada do Mouse (clique)
// =============================================================================
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) { // pode ser mantida
    ApplicationContext *context = (ApplicationContext*) glfwGetWindowUserPointer(window);
    World *world = context->world;
    Camera *camera = context->camera;
    RayCastResult rcResult = camera->detectBlock((*world), 10);
    if (rcResult.hit) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            world->removeBlock(rcResult.blockPos);
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            world->addBlock(rcResult.lastBlockPos);
        }
    }
}

// ============================================================================= 
// Callback para atualização do tamanho da janela
// =============================================================================
void framebuffer_size_callback(GLFWwindow* window, int width, int height) { // pode ser mantida
     glViewport(0, 0, width, height);

    ApplicationContext* context = static_cast<ApplicationContext*>(glfwGetWindowUserPointer(window));
    
    if (context && context->camera) {
        context->camera->updateProj((float)width, (float)height);
    }
}

int main() {
    // ============================================================================= 
    // Informa a direção em que as texturas devem ser carregadas
    // =============================================================================
    stbi_set_flip_vertically_on_load(true);

    // ============================================================================= 
    // Informa ao programa qual o diretório "raiz"
    // =============================================================================
    std::filesystem::path startDir = std::filesystem::current_path();

    // ============================================================================= 
    // Inicialização do GLFW
    // =============================================================================
    glfwInit();
    glfwWindowHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Sandbox", NULL, NULL);
    if (!window) {
        std::cout << "Falha ao criar a janela" << std::endl;
        glfwTerminate();
        return -1;
    }

    // ============================================================================= 
    // Configuração do GLFW
    // =============================================================================
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Falha ao inicializar o GLAD" << std::endl;
        return -1;
    }

    // ============================================================================= 
    // Ativação do modo de teste de profundidade
    // =============================================================================
    glEnable(GL_DEPTH_TEST);

    // ============================================================================= 
    // Inicialização e configuração do ImGui (Janela para depuração)
    // =============================================================================
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 330";
    ImGui_ImplOpenGL3_Init(glsl_version);

    // ============================================================================= 
    // Inicialização do mundo
    // =============================================================================
    World world;
    world.create();

    // ============================================================================= 
    // Inicialização do ponto de surgimento do mundo
    // =============================================================================
    glm::vec3 spawnPoint = glm::vec3(0.5f, CHUNK_HEIGHT, 0.5f);
    world.update(spawnPoint);

    // ============================================================================= 
    // Inicialização e posicionamento da câmera
    // =============================================================================
    for (int y = CHUNK_HEIGHT - 1; y>= 0; y--) {
        Vec3i pos = {0, y, 0};
        if (world.hasBlockAt(pos)) {
            spawnPoint = glm::vec3(0.5f, y, 0.5f);
            break;
        }
    }
    Camera camera(800.0f, 600.0f, spawnPoint);

    // ============================================================================= 
    // Configuração do contexto da aplicação (Usado para callbacks)
    // =============================================================================
    ApplicationContext context;
    context.world = &world;
    context.camera = &camera;
    glfwSetWindowUserPointer(window, &context);

    // ============================================================================= 
    // Inicialização dos shaders
    // =============================================================================
    std::optional<std::filesystem::path> cubeV = findPath(startDir, "cube.vert");
    std::optional<std::filesystem::path> cubeF = findPath(startDir, "cube.frag");
    std::optional<std::filesystem::path> bordersV = findPath(startDir, "borders.vert");
    std::optional<std::filesystem::path> bordersF = findPath(startDir, "borders.frag");
    if (!cubeV.has_value() && !cubeF.has_value() && bordersV.has_value() && !bordersF.has_value()) return -1;
    Shader shaderProgram((*cubeV).c_str(), (*cubeF).c_str());
    Shader borderShaderProgram((*bordersV).c_str(), (*bordersF).c_str());

    // ============================================================================= 
    // Inicialização das texturas
    // =============================================================================
    std::optional<std::filesystem::path> grass = findPath(startDir, "grass.jpg");
    std::optional<std::filesystem::path> dirt = findPath(startDir, "dirt.jpg");
    std::optional<std::filesystem::path> stone = findPath(startDir, "stone.jpg");
    std::optional<std::filesystem::path> water = findPath(startDir, "water.jpg");
    std::optional<std::filesystem::path> rootstone = findPath(startDir, "rootstone.jpg");
    if (!grass.has_value() && !dirt.has_value() && !stone.has_value() && !water.has_value() && !rootstone.has_value()) return -1;
    Texture grassTexture((*grass).c_str());
    Texture dirtTexture((*dirt).c_str());
    Texture stoneTexture((*stone).c_str());
    Texture waterTexture((*water).c_str());
    Texture rootstoneTexture((*rootstone).c_str());

    // ============================================================================= 
    // Inicialização e configuração da "mira" da câmera
    // =============================================================================
    unsigned int crosshairVAO, crosshairVBO;
    glGenVertexArrays(1, &crosshairVAO);
    glGenBuffers(1, &crosshairVBO);
    glBindVertexArray(crosshairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
    float crosshairVertices[] = {
        // Linha Horizontal
        -10.0f,  0.0f, 0.0f,
         10.0f,  0.0f, 0.0f,
        // Linha Vertical
         0.0f, -10.0f, 0.0f,
         0.0f,  10.0f, 0.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    std::optional<std::filesystem::path> crosshairV = findPath(startDir, "crosshair.vert");
    std::optional<std::filesystem::path> crosshairF = findPath(startDir, "crosshair.frag");
    if (!crosshairV.has_value() && !crosshairF.has_value()) return -1;
    Shader crosshairShaderProgram((*crosshairV).c_str(), (*crosshairF).c_str());

    // ============================================================================= 
    // Inicialização do depurador
    // =============================================================================
    Debugger debug;
    
    // ============================================================================= 
    // Configuração de váriaveis do programa
    // =============================================================================
    float deltaTime = 0.05f;
    float angle = 0.0f;

    // ============================================================================= 
    // Inicialização do laço principal do programa
    // =============================================================================
    while (!glfwWindowShouldClose(window)) {

        // ============================================================================= 
        // Captura de dados de entrada do programa (Mouse/Teclado)
        // =============================================================================
        glfwPollEvents();

        // ============================================================================= 
        // Atualização dos dados da janela de depuração
        // =============================================================================
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("Painel de Debug"); // Cria uma janela chamada "Painel de Debug"
            
            ImGui::Text(debug.camInfo.c_str());
            ImGui::Text(debug.chunkInfo.c_str());
            ImGui::Text(debug.seedInfo.c_str());
            ImGui::Text(debug.blockPosInfo.c_str());
            ImGui::Text(debug.blockTypeInfo.c_str());
            
            // Exemplo com FPS
            ImGui::Text("Aplicação com média de %.3f ms/quadro (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

            ImGui::End();
        }

        // ============================================================================= 
        // Mapeamento dos dados de entrada do programa
        // =============================================================================
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
             glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Libera o cursor
            io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Trava o cursor
            io.ConfigFlags |= ImGuiConfigFlags_NoMouse; // ImGui ignora o mouse
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera.processKeyboard(FORWARD, deltaTime*3);
        } else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera.processKeyboard(FORWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera.processKeyboard(BACKWARD, deltaTime*3);
        } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {    
            camera.processKeyboard(BACKWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.processKeyboard(LEFT, deltaTime*3);
        } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.processKeyboard(LEFT, deltaTime);
        } 
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.processKeyboard(RIGHT, deltaTime*3);
        } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.processKeyboard(RIGHT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            camera.processKeyboard(UP, deltaTime*1.5);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            //Vec3i pos = { (int)camera.getPos().x, (int)camera.getPos().y - 2, (int)camera.getPos().z };
            //if (!world.hasBlockAt(pos)) {
                camera.processKeyboard(DOWN, deltaTime*1.5);
            //}
        }

        // ============================================================================= 
        // Atualização dos vetores em câmera
        // =============================================================================
        camera.updateCamera();

        // ============================================================================= 
        // Limpeza da tela
        // =============================================================================
        glClearColor(0.53f, 0.81f, 0.92f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // ============================================================================= 
        // Configuração da iluminação e textura dos objetos
        // =============================================================================
        shaderProgram.use();
        glm::vec3 lightPosition(camera.getPos());
        shaderProgram.setVec3("lightPos", lightPosition.x, lightPosition.y, lightPosition.z);
        shaderProgram.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        angle += 0.01f;
        shaderProgram.setInt("blockTextures[0]", 0);
        shaderProgram.setInt("blockTextures[1]", 1);
        shaderProgram.setInt("blockTextures[2]", 2);
        shaderProgram.setInt("blockTextures[3]", 3);
        shaderProgram.setInt("blockTextures[4]", 4);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture.ID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, dirtTexture.ID);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, stoneTexture.ID);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, rootstoneTexture.ID);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, waterTexture.ID);

        // ============================================================================= 
        // Atualização das coordenadas no mundo para carregamento de chunks
        // =============================================================================
        world.update(camera.getPos());

        // ============================================================================= 
        // Renderização dos objetos, foco do objeto e renderização das bordas do chunk
        // =============================================================================
        world.draw(shaderProgram, camera.getProj(), camera.getView(), camera.getPos());
        RayCastResult rcResult = camera.detectBlock(world, 10);
        if (rcResult.hit) {
            world.highlight(borderShaderProgram, rcResult.blockPos, camera.getProj(), camera.getView());
        }
        // debug.drawChunkGrid(camera, camera.getProj(), camera.getView());

        // ============================================================================= 
        // Renderização da janela de depuração
        // =============================================================================
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                
        // ============================================================================= 
        // ?
        // =============================================================================
        glClear(GL_DEPTH_BUFFER_BIT);
        
        // ============================================================================= 
        // Renderização da "mira" da câmera
        // =============================================================================
        crosshairShaderProgram.use();
        glm::mat4 projOrtho = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(800.0f / 2.0f, 600.0f / 2.0f, 0.0f));
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 transform = projOrtho * view * model;
        crosshairShaderProgram.setMat4("transform", transform);
        glBindVertexArray(crosshairVAO);
        glDrawArrays(GL_LINES, 0, 4);
        glBindVertexArray(0);
        
        // ============================================================================= 
        // ?
        // =============================================================================
        glfwSwapBuffers(window);
        
        // ============================================================================= 
        // Atualização dos dados para a janela de depuração
        // =============================================================================
        int chunkX = static_cast<int>(floor((float)camera.getPos().x/CHUNK_WIDTH));
        int chunkY = static_cast<int>(floor((float)camera.getPos().y/CHUNK_HEIGHT));
        int chunkZ = static_cast<int>(floor((float)camera.getPos().z/CHUNK_DEPTH));
        Vec3i chunkPos = { chunkX, chunkY, chunkZ };
        Chunk* temp_chunk = world.getChunk(chunkPos.x, chunkPos.y, chunkPos.z);      
        debug.updateBlockInfo(world, rcResult.blockPos);
        debug.updateCamInfo(camera);
        debug.updateFPS();
        debug.updateSeed(world.getSeed());
        debug.updateChunkInfo(*temp_chunk);
    }

    // ============================================================================= 
    // Finalização do programa
    // =============================================================================
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
