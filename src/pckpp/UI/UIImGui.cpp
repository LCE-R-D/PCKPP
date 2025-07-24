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

    ImGuiStyle& style = ImGui::GetStyle();
    style.CellPadding = ImVec2(0, 0);

    ImFontConfig config;
    config.MergeMode = false;
    config.PixelSnapH = true;

    ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/fonts/ark-pixel-12px-monospaced-latin.ttf", 18.0f, &config);

    config.MergeMode = true;

    // Merge Chinese (Simplified)
    ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/fonts/ark-pixel-12px-monospaced-zh_cn.ttf", 18.0f, &config, ImGui::GetIO().Fonts->GetGlyphRangesChineseSimplifiedCommon());
    // Merge Chinese (Traditional)
    ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/fonts/ark-pixel-12px-monospaced-zh_tw.ttf", 18.0f, &config, ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());
    // Merge Japanese
    ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/fonts/ark-pixel-12px-monospaced-ja.ttf", 18.0f, &config, ImGui::GetIO().Fonts->GetGlyphRangesJapanese());
    // Merge Korean
    ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/fonts/ark-pixel-12px-monospaced-ko.ttf", 18.0f, &config, ImGui::GetIO().Fonts->GetGlyphRangesKorean());

    ImGui::GetIO().Fonts->Build();
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

void UIImGui::RenderFileTree()
{ }