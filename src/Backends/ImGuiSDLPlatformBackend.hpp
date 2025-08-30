#pragma once

#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>
#include "Backends/PlatformBackend.h"

class ImGuiSDLPlatformBackend : public PlatformBackend {
public:
    // Inits the platform backend
    bool Init(void* platformData) override {
        SDL_Window* window = static_cast<SDL_Window*>(platformData);
        return ImGui_ImplSDL3_InitForOpenGL(window, nullptr);
    }

    // Processes all events in the platform backend
    void ProcessEvent(void* event) override {
        ImGui_ImplSDL3_ProcessEvent(static_cast<SDL_Event*>(event));
    }

    // Runs the new frame event
    void NewFrame() override {
        ImGui_ImplSDL3_NewFrame();
    }

    // Runs the shut down / cleanup event
    void Shutdown() override {
        ImGui_ImplSDL3_Shutdown();
    }
};