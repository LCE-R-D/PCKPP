#pragma once

#include <memory>
#include <imgui.h>
#include "Backends/PlatformBackend.h"
#include "Backends/RendererBackend.h"
#include "UI/UIBase.h"

class UIImGui : public UIBase {
public:
    UIImGui() = default;
    ~UIImGui() = default;

    // Initializes ImGui
    bool Init() override;

    // Initalizes SDL and OpenGL backends for ImGui
    bool InitBackends(void* platformData, void* rendererData);

    // Processes ImGui events
    void ProcessEvent(void* event) override;

    // Starts a new ImGui frame
    void NewFrame() override;

    // Renders ImGui frame data
    void Render() override;

    // Runs ImGui shutdown/clean up event
    void Shutdown() override;

    // Renders the file tree in the main program form using ImGui elements
    void RenderFileTree() override;

    // Renders the menu bar in the window using ImGui elements
    void RenderMenuBar() override;

    // Renders the context menu in the main program form's file tree using ImGui elements
    void RenderContextMenu(FileTreeNode& node) override;

    // Renders the preview window in the main program form using ImGui elements, takes file to preview
    void RenderPreviewWindow(PCKAssetFile& file) override;

    // Renders the properties window in the main program form using ImGui elements, takes file to get properties from lol
    void RenderPropertiesWindow(PCKAssetFile& file) override;

    // Renders a node on the node tree using ImGui elements; TODO: probably clean up the function's parameters. This seems unnecessary.
    void RenderNode(FileTreeNode& node, std::vector<const FileTreeNode*>* visibleList = nullptr, bool shouldScroll = false, bool openFolder = false, bool closeFolder = false) override;

    // Shows a modal pop up to handle dropped file types with numerous actions for the same extension using ImGui elements
    void ShowFileDropPopUp(const std::string& filepath) override;

    // Handles keyboard input using ImGui and SDL, respectively
    void HandleInput() override;
};

// Helpful opertaors for ImVec2

inline ImVec2 operator+(const ImVec2& a, const ImVec2& b) {
    return ImVec2(a.x + b.x, a.y + b.y);
}

inline ImVec2 operator-(const ImVec2& a, const ImVec2& b) {
    return ImVec2(a.x - b.x, a.y - b.y);
}

inline ImVec2 operator*(const ImVec2& a, float s) {
    return ImVec2(a.x * s, a.y * s);
}

inline ImVec2 operator*(float s, const ImVec2& a) {
    return ImVec2(a.x * s, a.y * s);
}

inline ImVec2 operator/(const ImVec2& a, float s) {
    return ImVec2(a.x / s, a.y / s);
}

inline ImVec2& operator+=(ImVec2& a, const ImVec2& b) {
    a.x += b.x; a.y += b.y; return a;
}

inline ImVec2& operator-=(ImVec2& a, const ImVec2& b) {
    a.x -= b.x; a.y -= b.y; return a;
}

inline ImVec2& operator*=(ImVec2& a, float s) {
    a.x *= s; a.y *= s; return a;
}

inline ImVec2& operator/=(ImVec2& a, float s) {
    a.x /= s; a.y /= s; return a;
}