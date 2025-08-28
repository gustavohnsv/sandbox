#define STB_IMAGE_IMPLEMENTATION
#include "../Other/stb_image.h"
#include "texture.h"

Texture::Texture(const char* texture) {
    glGenTextures(1, &this->ID);
    int width, height, nrComponents;
    unsigned char* data = stbi_load(texture, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 3) format = GL_RGB;
        else if (nrComponents == 4) format = GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, this->ID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        stbi_image_free(data);
    } else {
        std::cout << "Falha ao carregar texturas: " << texture << std::endl;
    }
    check();
}

Texture::~Texture() {
    // Funções GLFW
    glDeleteTextures(1, &ID);
    check();
}

void Texture::bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::check() {
    checkOpenGLError();
}