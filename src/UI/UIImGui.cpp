#include "../Application/Application.h"
#include "UIImGui.h"

bool UIImGui::Init() {
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    return true;
}

bool UIImGui::InitBackends(void* platformData, void* rendererData) {
    PlatformBackend* platformBackend = gApp->GetPlatformBackend();
    RendererBackend* rendererBackend = gApp->GetRendererBackend();

    if (platformBackend && !platformBackend->Init(platformData))
        return false;
    if (rendererBackend && !rendererBackend->Init(rendererData)) {
        if (platformBackend)
            platformBackend->Shutdown();
        return false;
    }
    return true;
}

void UIImGui::ProcessEvent(void* event) {
    PlatformBackend* platformBackend = gApp->GetPlatformBackend();

    if (platformBackend)
        platformBackend->ProcessEvent(event);
}

void UIImGui::NewFrame() {
    PlatformBackend* platformBackend = gApp->GetPlatformBackend();
    RendererBackend* rendererBackend = gApp->GetRendererBackend();

    if (platformBackend)
        platformBackend->NewFrame();
    if (rendererBackend)
        rendererBackend->NewFrame();
    ImGui::NewFrame();
}

void UIImGui::Render() {
    RendererBackend* rendererBackend = gApp->GetRendererBackend();

    ImGui::Render();
    if (rendererBackend)
        rendererBackend->Render();
}

void UIImGui::Shutdown() {
    PlatformBackend* platformBackend = gApp->GetPlatformBackend();
    RendererBackend* rendererBackend = gApp->GetRendererBackend();

    if (rendererBackend) {
        rendererBackend->Shutdown();
    }
    if (platformBackend) {
        platformBackend->Shutdown();
    }
    ImGui::DestroyContext();
}
