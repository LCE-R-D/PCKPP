#pragma once

#include <string>
#include "Backends/RendererBackend.h"

struct Texture
{
    unsigned int id{0};
    int width{0};
    int height{0};
};

enum class TextureFilter
{
    NEAREST,
    LINEAR,
    NEAREST_MIPMAP_NEAREST,
    LINEAR_MIPMAP_NEAREST,
    NEAREST_MIPMAP_LINEAR,
    LINEAR_MIPMAP_LINEAR
};

class GraphicsBase {
public:
    virtual ~GraphicsBase() {}

    // Initializes the Graphics framework
    virtual bool Init() = 0;

    // Runs the shut down / cleanup event
    virtual void Shutdown() = 0;

    // Starts a new frame to present to screen
    virtual void NewFrame() = 0;

    // Renders data to the current frame
    virtual void Render() = 0;

    // Presents final render data to screen
    virtual void Present() = 0;

    // Loads texture from memory/data
    virtual Texture LoadTextureFromMemory(const void* data, size_t size, TextureFilter filter) = 0;

    // Loads texture from file
    virtual Texture LoadTextureFromFile(const std::string& path, TextureFilter filter) = 0;

    // Deletes texture
    virtual void DeleteTexture(const Texture& texture) = 0;
};