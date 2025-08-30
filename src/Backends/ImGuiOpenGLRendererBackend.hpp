#pragma once

#include <backends/imgui_impl_opengl2.h>
#include "Backends/RendererBackend.h"

class ImGuiOpenGLRendererBackend : public RendererBackend {
public:
    // Inits the renererer backend
    bool Init(void* rendererData = nullptr) override {
        // data not needed in this case
        return ImGui_ImplOpenGL2_Init();
    }

    // Runs the render event
    void Render() override {
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    }

    // Runs the new frame event
    void NewFrame() override {
        ImGui_ImplOpenGL2_NewFrame();
    }

    // Runs the shut down / cleanup event
    void Shutdown() override {
        ImGui_ImplOpenGL2_Shutdown();
    }
};