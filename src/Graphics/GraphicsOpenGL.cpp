#include <iostream>
#include <fstream>
#include <vector>
#include "Graphics/GraphicsOpenGL.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

GraphicsOpenGL::GraphicsOpenGL() = default;

GraphicsOpenGL::~GraphicsOpenGL() {
    Shutdown();
}

bool GraphicsOpenGL::Init() {
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }

    return true;
}

void GraphicsOpenGL::Shutdown() {

}

void GraphicsOpenGL::NewFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GraphicsOpenGL::Render() {

}

void GraphicsOpenGL::Present() {

}

Texture GraphicsOpenGL::LoadTextureFromMemory(const void* data, size_t size, TextureFilter filter) {
    int width, height, channels;
    stbi_uc* pixels = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(data), static_cast<int>(size), &width, &height, &channels, 4);
    if (!pixels) {
        std::cerr << "Failed to load texture from memory\n";
        return {};
    }

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    GLint glFilter = GetGLFilter(filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // only generate textures with mipmaps enabled
    if (glFilter == GL_LINEAR_MIPMAP_LINEAR ||
        glFilter == GL_NEAREST_MIPMAP_LINEAR ||
        glFilter == GL_LINEAR_MIPMAP_NEAREST ||
        glFilter == GL_NEAREST_MIPMAP_NEAREST)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, texID);

    stbi_image_free(pixels);

    return { texID, width, height };
}

Texture GraphicsOpenGL::LoadTextureFromFile(const std::string& path, TextureFilter filter) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
        printf("Failed to open image file: %s\n", path.c_str());
        return {};
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        printf("Failed to read image file: %s\n", path.c_str());
        return {};
    }

    printf("Loaded image file: %s\n", path.c_str());

    return LoadTextureFromMemory(buffer.data(), size, filter);
}

GLint GraphicsOpenGL::GetGLFilter(TextureFilter filter) const {
    switch (filter)
    {
    default:
    case TextureFilter::NEAREST:
        return GL_NEAREST;
    case TextureFilter::LINEAR:
        return GL_LINEAR;
    case TextureFilter::NEAREST_MIPMAP_NEAREST:
        return GL_NEAREST_MIPMAP_NEAREST;
    case TextureFilter::LINEAR_MIPMAP_NEAREST:
        return GL_LINEAR_MIPMAP_NEAREST;
    case TextureFilter::NEAREST_MIPMAP_LINEAR:
        return GL_NEAREST_MIPMAP_LINEAR;
    case TextureFilter::LINEAR_MIPMAP_LINEAR:
        return GL_LINEAR_MIPMAP_LINEAR;
    }
}

void GraphicsOpenGL::DeleteTexture(const Texture& texture)
{
    glDeleteTextures(1, &texture.id);
}