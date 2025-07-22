#pragma once
#include "UIBase.h"
#include "../Backends/PlatformBackend.h"
#include "../Backends/RendererBackend.h"
#include <imgui.h>
#include <memory>

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
};

// Helpful opertaors for Vector stuff

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