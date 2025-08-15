#include "Other/stb_image.h"

#include "Common/main.h"
#include "Shaders/main.h"
#include "Camera/main.h"
#include "World/main.h"
#include "Debugger/main.h"
#include "Texture/main.h"

struct ApplicationContext {
    Camera *camera;
    World *world;
};

std::string loadFile(const std::string &path) { // pode ser removida
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Não foi possível abrir o arquivo!");
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) { // pode ser mantida
    ApplicationContext *context = (ApplicationContext*) glfwGetWindowUserPointer(window);
    Camera *camera = context->camera;
    if (camera) {
        camera->processMouseMovement(xpos, ypos);
    }
}

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { // pode ser mantida
    glViewport(0, 0, width, height);
}

int main() {
    stbi_set_flip_vertically_on_load(true);
    // Inicialização GLFW
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

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Falha ao inicializar o GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Inicializa o mundo
    World world(5);
    world.create();

    // Inicializa a câmera
    // obs: por que camera com as coordenadas {0, 0, 0} aparece exatamente em cima do bloco?
    int groundY = 0;
    for (int y = CHUNK_HEIGHT - 1; y>= 0; y--) {
        Vec3i pos = {0, y, 0};
        if (world.hasBlockAt(pos)) {
            groundY = y;
            break;
        }
    }
    Camera camera(800.0f, 600.0f, glm::vec3(0.0f, (float)groundY+2, 0.0f));
    
    ApplicationContext context;
    context.world = &world;
    context.camera = &camera;

    glfwSetWindowUserPointer(window, &context);
    
    Shader shaderProgram("shaders/cube.vert", "shaders/cube.frag");
    Shader borderShaderProgram("shaders/borders.vert", "shaders/borders.frag");

    Texture grassTexture("textures/grass.jpg");
    Texture dirtTexture("textures/dirt.jpg");
    Texture stoneTexture("textures/stone.jpg");
    Texture rootstoneTexture("textures/rootstone.jpg");

    unsigned int crosshairVAO, crosshairVBO;
    glGenVertexArrays(1, &crosshairVAO);
    glGenBuffers(1, &crosshairVBO);

    glBindVertexArray(crosshairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);

    // Vértices para duas linhas: uma horizontal e uma vertical
    // Coordenadas são relativas ao centro (0,0)
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

    Shader crosshairShaderProgram("shaders/crosshair.vert", "shaders/crosshair.frag");

    // unsigned int lightColorLoc = glGetUniformLocation(shaderProgram.ID, "lightColor");
    // unsigned int lightDirLoc = glGetUniformLocation(shaderProgram.ID, "lightDir");


    float timeSinceLastUpdate = 0.0f;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    int frameCount = 0;

    // Dentro do loop principal
    float angle = 0.0f;
    while (!glfwWindowShouldClose(window)) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        frameCount++;
        timeSinceLastUpdate += deltaTime;
        if (timeSinceLastUpdate >= 1.0f) {
            std::cout << "FPS atual: " << frameCount << std::endl;
            frameCount = 0;
            timeSinceLastUpdate = 0.0f;
        }

        glfwPollEvents();

        camera.updateCamera();

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera.processKeyboard(FORWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera.processKeyboard(BACKWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.processKeyboard(LEFT, deltaTime);
        } 
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.processKeyboard(RIGHT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            camera.processKeyboard(UP, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            Vec3i pos = { (int)camera.getPos().x, (int)camera.getPos().y - 2, (int)camera.getPos().z };
            if (!world.hasBlockAt(pos)) {
                camera.processKeyboard(DOWN, deltaTime);
            }
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            camera.processKeyboard(FORWARD, deltaTime*3);
        }

        glClearColor(0.53f, 0.81f, 0.92f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RayCastResult rcResult = camera.detectBlock(world, 10);
        
        shaderProgram.use();

        glm::vec3 lightPosition(camera.getPos());
        shaderProgram.setVec3("lightPos", lightPosition.x, lightPosition.y, lightPosition.z);
        shaderProgram.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        angle += 0.01f;

        shaderProgram.setInt("blockTextures[0]", 0);
        shaderProgram.setInt("blockTextures[1]", 1);
        shaderProgram.setInt("blockTextures[2]", 2);
        shaderProgram.setInt("blockTextures[3]", 3);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture.ID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, dirtTexture.ID);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, stoneTexture.ID);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, rootstoneTexture.ID);

        world.draw(shaderProgram, camera.getProj(), camera.getView(), camera.getPos());

        if (rcResult.hit) {
            // vai virar função da classe Debugger
            //std::cout << "Bloco detectado" << std::endl;
            world.highlight(borderShaderProgram, rcResult.blockPos, camera.getProj(), camera.getView());
        }

        glClear(GL_DEPTH_BUFFER_BIT);

        crosshairShaderProgram.use();
        glm::mat4 projOrtho = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(800.0f / 2.0f, 600.0f / 2.0f, 0.0f));
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 transform = projOrtho * view * model;
        crosshairShaderProgram.setMat4("transform", transform);
        glBindVertexArray(crosshairVAO);
        glDrawArrays(GL_LINES, 0, 4);
        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
