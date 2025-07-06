#pragma once
#include "imgui.h"

// This is basically just to save time lol

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