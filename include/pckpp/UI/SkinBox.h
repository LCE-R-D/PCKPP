#pragma once
#include <pckpp/Graphics/GraphicsOpenGL.h>

struct UVRect
{
    float u0, v0; // top-left UV
    float u1, v1; // bottom-right UV
};

class Box
{
public:
    int textureWidth, textureHeight;
    float x, y, z;
    float length, height, width;
    int u, v;
    int armorMask;
    bool mirrored;
    float scale;

    UVRect front, back, top, bottom, right, left;

    Box(int textureWidth, int textureHeight, float x, float y, float z, float width, float height, float length, float u, float v, int armorMask = 0, bool mirrored = false, float scale = 0.0f)
        : x(x), y(y), z(z), width(width), height(height), length(width), u(u), v(v), armorMask(armorMask), mirrored(mirrored), scale(scale),
        textureWidth(textureWidth), textureHeight(textureHeight)
    {
        // Normalize inputs
        float u0 = u / textureWidth;
        float v0 = v / textureHeight;
        float uvLength = length / textureWidth;
        float uvWidth = width / textureWidth;
        float uvHeight = height / textureHeight;

        left = { u0, v0 + uvHeight, u0 + uvWidth, v0 + uvHeight * 2 };

        front = { left.u1, left.v0, left.u1 + uvLength, left.v1 };

        right = { front.u1, left.v0, front.u1 + uvWidth, left.v1 };

        back = { right.u1, left.v0, right.u1 + uvLength, left.v1 };

        top = { front.u0, v0, front.u1, v0 + uvHeight };

        bottom = { right.u0, v0, right.u1, v0 + uvHeight };
    }

    void Draw() const
    {
        glBegin(GL_QUADS);

        // Front
        glTexCoord2f(front.u0, front.v1); glVertex3f(-length, -height, width);
        glTexCoord2f(front.u1, front.v1); glVertex3f(length, -height, width);
        glTexCoord2f(front.u1, front.v0); glVertex3f(length, height, width);
        glTexCoord2f(front.u0, front.v0); glVertex3f(-length, height, width);

        // Back
        glTexCoord2f(back.u1, back.v1); glVertex3f(-length, -height, -width);
        glTexCoord2f(back.u1, back.v0); glVertex3f(-length, height, -width);
        glTexCoord2f(back.u0, back.v0); glVertex3f(length, height, -width);
        glTexCoord2f(back.u0, back.v1); glVertex3f(length, -height, -width);

        // Top
        glTexCoord2f(top.u0, top.v1); glVertex3f(-length, height, -width);
        glTexCoord2f(top.u0, top.v0); glVertex3f(-length, height, width);
        glTexCoord2f(top.u1, top.v0); glVertex3f(length, height, width);
        glTexCoord2f(top.u1, top.v1); glVertex3f(length, height, -width);

        // Bottom
        glTexCoord2f(bottom.u1, bottom.v1); glVertex3f(-length, -height, -width);
        glTexCoord2f(bottom.u0, bottom.v1); glVertex3f(length, -height, -width);
        glTexCoord2f(bottom.u0, bottom.v0); glVertex3f(length, -height, width);
        glTexCoord2f(bottom.u1, bottom.v0); glVertex3f(-length, -height, width);

        // Right
        glTexCoord2f(right.u1, right.v1); glVertex3f(length, -height, -width);
        glTexCoord2f(right.u1, right.v0); glVertex3f(length, height, -width);
        glTexCoord2f(right.u0, right.v0); glVertex3f(length, height, width);
        glTexCoord2f(right.u0, right.v1); glVertex3f(length, -height, width);

        // Left
        glTexCoord2f(left.u0, left.v1); glVertex3f(-length, -height, -width);
        glTexCoord2f(left.u1, left.v1); glVertex3f(-length, -height, width);
        glTexCoord2f(left.u1, left.v0); glVertex3f(-length, height, width);
        glTexCoord2f(left.u0, left.v0); glVertex3f(-length, height, -width);

        glEnd();
    }
};