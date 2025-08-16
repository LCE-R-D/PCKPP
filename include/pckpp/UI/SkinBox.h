#pragma once
#include <pckpp/Graphics/GraphicsOpenGL.h>

struct UVRect
{
    float u0, v0; // top-left UV
    float u1, v1; // bottom-right UV
};

class SkinBox
{
public:
    int textureWidth, textureHeight;
    float x, y, z;
    float width, height, depth;
    int u, v;
    int armorMask;
    bool mirrored;
    float scale{0.0f};

    UVRect front, back, top, bottom, right, left;

    SkinBox(Texture& tex, float x, float y, float z, float width, float height, float depth, float u, float v, int armorMask = 0, bool mirrored = false, float scale = 0.0f)
        : x(x), y(y), z(z), width(width), height(height), depth(depth), u(u), v(v), armorMask(armorMask), mirrored(mirrored), scale(scale), textureWidth(tex.width), textureHeight(tex.height)
    {
        // Normalize variables
        float u0 = u / textureWidth;
        float v0 = v / textureHeight;
        float uvWidth = width / textureWidth;
        float uvHeight = height / textureHeight;
        float uvDepthX = depth / textureWidth;
        float uvDepthY = depth / textureHeight;

        top = { u0 + uvDepthX, v0, u0 + uvWidth + uvDepthX, v0 + uvDepthY };
        bottom = { top.u1, top.v0, top.u1 + uvWidth, top.v1 };
        left = { u0, top.v1, top.u0, v0 + uvDepthY + uvHeight};
        front = { top.u0, top.v1, top.u1, left.v1};
        right = { top.u1, top.v1, top.u1 + uvDepthX, left.v1};
        back = { right.u1, top.v1, right.u1 + uvWidth, left.v1};
    }

    void Draw() const
    {
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBegin(GL_QUADS);

        float x0 = x - scale;
        float x1 = x + width + scale;
        float y0 = -y - scale;
        float y1 = -y + height + scale;
        float z0 = z - scale;
        float z1 = z + depth + scale;

        if (mirrored) std::swap(x0, x1);

        glTexCoord2f(front.u0, front.v1); glVertex3f(x0, y0, z1);
        glTexCoord2f(front.u1, front.v1); glVertex3f(x1, y0, z1);
        glTexCoord2f(front.u1, front.v0); glVertex3f(x1, y1, z1);
        glTexCoord2f(front.u0, front.v0); glVertex3f(x0, y1, z1);

        glTexCoord2f(back.u1, back.v1); glVertex3f(x0, y0, z0);
        glTexCoord2f(back.u1, back.v0); glVertex3f(x0, y1, z0);
        glTexCoord2f(back.u0, back.v0); glVertex3f(x1, y1, z0);
        glTexCoord2f(back.u0, back.v1); glVertex3f(x1, y0, z0);

        glTexCoord2f(top.u0, top.v0); glVertex3f(x0, y1, z0);
        glTexCoord2f(top.u0, top.v1); glVertex3f(x0, y1, z1);
        glTexCoord2f(top.u1, top.v1); glVertex3f(x1, y1, z1);
        glTexCoord2f(top.u1, top.v0); glVertex3f(x1, y1, z0);

        glTexCoord2f(bottom.u1, bottom.v1); glVertex3f(x0, y0, z0);
        glTexCoord2f(bottom.u0, bottom.v1); glVertex3f(x1, y0, z0);
        glTexCoord2f(bottom.u0, bottom.v0); glVertex3f(x1, y0, z1);
        glTexCoord2f(bottom.u1, bottom.v0); glVertex3f(x0, y0, z1);

        glTexCoord2f(right.u1, right.v1); glVertex3f(x1, y0, z0);
        glTexCoord2f(right.u1, right.v0); glVertex3f(x1, y1, z0);
        glTexCoord2f(right.u0, right.v0); glVertex3f(x1, y1, z1);
        glTexCoord2f(right.u0, right.v1); glVertex3f(x1, y0, z1);

        glTexCoord2f(left.u0, left.v1); glVertex3f(x0, y0, z0);
        glTexCoord2f(left.u1, left.v1); glVertex3f(x0, y0, z1);
        glTexCoord2f(left.u1, left.v0); glVertex3f(x0, y1, z1);
        glTexCoord2f(left.u0, left.v0); glVertex3f(x0, y1, z0);

        glEnd();

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    }
};