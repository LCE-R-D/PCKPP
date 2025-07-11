#pragma once

#include "../PCK/PCKAssetFile.h"
#include <imgui.h>
#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_opengl2.h>
#include <glad/glad.h>

struct Texture {
    GLuint id{0};
    int width{0};
    int height{0};
};

// Loads texture from memory; also takes optional GL Filter Parameter
Texture LoadTextureFromMemory(const void* data, size_t size, int glFilter = GL_NEAREST);

// Loads texture from file path; also takes optional GL Filter Parameter
Texture LoadTextureFromFile(const std::string& path, int glFilter = GL_NEAREST);

// Gets the current SDL Window
SDL_Window* GetWindow();

// Just checks to see if quit was called somewhere
bool shouldClose();

// Setup Graphic related vendors
bool GraphicsSetup();

// Cleanup Graphic related vendors
void GraphicsCleanup();