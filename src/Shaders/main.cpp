#include "main.h"

Shader::Shader(const char* vertex, const char* frag) {
    // 1. LER OS ARQUIVOS DE SHADER
    std::ifstream vFile;
    std::ifstream fFile;
    std::string vCode;
    std::string fCode;
    vFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        vFile.open(vertex);
        fFile.open(frag);
        std::stringstream vStream, fStream;
        vStream << vFile.rdbuf();
        fStream << fFile.rdbuf();
        vFile.close();
        fFile.close();
        vCode = vStream.str();
        fCode = fStream.str();
    } catch (std::ifstream::failure &e) {
        std::cout << "[Erro]: Erro ao carregar arquivos de shader." << std::endl;
    } 

    // 2. COMPILAR OS SHADERS
    const char* vSource = vCode.c_str();
    const char* fSource = fCode.c_str();
    unsigned int vID, fID;
    vID = glCreateShader(GL_VERTEX_SHADER);
    fID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vID, 1, &vSource, NULL);
    glShaderSource(fID, 1, &fSource, NULL);
    glCompileShader(vID);
    glCompileShader(fID);
    check(vID, "VERTEX");
    check(fID, "FRAGMENT");
    
    // 3. LINKAR OS SHADERS 
    ID = glCreateProgram();
    glAttachShader(ID, vID);
    glAttachShader(ID, fID);
    glLinkProgram(ID);
    check(ID, "PROGRAM");

    // 4. DELETAR SHADERS
    glDeleteShader(vID);
    glDeleteShader(fID);

}

void Shader::use() {
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string &name, const glm::mat4 value) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::setVec3(const std::string &name, float v1, float v2, float v3) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), v1, v2, v3);
}

void Shader::check(unsigned int shader, std::string type) {
    return;
}
