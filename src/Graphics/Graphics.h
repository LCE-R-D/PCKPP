#pragma once

#include "../PCK/PCKAssetFile.h"
#include <imgui.h>
#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_opengl2.h>
#include <glad/glad.h>

struct Texture {
    GLuint id;
    int width;
    int height;
};

Texture LoadTextureFromMemory(const void* data, size_t size);

void PreviewImage(const PCKAssetFile& file);