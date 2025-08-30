#pragma once

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include "Graphics/GraphicsBase.h"

class GraphicsOpenGL : public GraphicsBase {
public:
    GraphicsOpenGL();
    ~GraphicsOpenGL() override;

    // Initalizes OpenGL and GLAD
    bool Init() override;

    // Shut fown the OpenGL and GLAD frameworks
    void Shutdown() override;

    // Starts a new frame to present to screen
    void NewFrame() override;

    // Renders data to the current frame
    void Render() override;

    // Presents the final render data to window
    void Present() override;

    // Loads textures from memory/data
    Texture LoadTextureFromMemory(const void* data, size_t size, TextureFilter filter = TextureFilter::NEAREST) override;

    // Loads textures from file, like TGA, PNG
    Texture LoadTextureFromFile(const std::string& path, TextureFilter filter = TextureFilter::NEAREST) override;

    void DeleteTexture(const Texture& texture) override;

    // convert filter to GL Filter
    GLint GetGLFilter(TextureFilter filter) const;

private:
    bool mShouldClose = false;
};