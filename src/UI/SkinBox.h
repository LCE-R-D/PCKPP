#pragma once
#include "Graphics/GraphicsOpenGL.h"
#include "Util/Util.h"

struct UVRect
{
    float u0, v0; // top-left UV
    float u1, v1; // bottom-right UV
};

int textureWidth, textureHeight;
bool mirroredBottom;

class SkinBox
{
public:
    std::string type{};
    float x, y, z;
    float width, height, depth;
    float u, v;
    int armorMask;
    bool mirrored;
    float scale{ 0.0f };

    SkinBox() : SkinBox("BODY", 0, 0, 0, 0, 0, 0, 0, 0, 0, false, 0) {}

    SkinBox(const std::string& type, float x, float y, float z, float width, float height, float depth, float u, float v, int armorMask = 0, bool mirrored = false, float scale = 0.0f)
        : type(type), x(x), y(y), z(z), width(width), height(height), depth(depth), u(u), v(v), armorMask(armorMask), mirrored(mirrored), scale(scale)
    {
        calculateUVs();
    }

    void parse(const std::wstring& value) {
        std::wstringstream boxValue(value);

        std::wstring typeWide;

        boxValue >> typeWide
            >> x >> y >> z
            >> width >> height >> depth
            >> u >> v >> armorMask >> mirrored >> scale;

        type = Binary::ToUTF8({ typeWide.begin(), typeWide.end() });

        calculateUVs();
    }

    // for simplifying layer box creation
    static SkinBox CreateLayer(SkinBox& other, int u, int v, float scale)
    {
        return SkinBox(other.type, other.x, other.y, other.z, other.width, other.height, other.depth, u, v, 0, false, scale);
    }

    void Draw() const
    {
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBegin(GL_QUADS);

        float offsetX{};
        float offsetY{};
        float offsetZ{};

        if (type == "HEAD_DEFAULT")
        {
            offsetY = -4;
        }
        if (type == "HEAD")
        {
            offsetY = -8;
        }
        if (type == "BODY")
        {
            offsetY = 2;
        }
        if (type == "ARM0")
        {
            offsetX = -5;
            offsetY = 2;
        }
        if (type == "ARM1")
        {
            offsetX = 5;
            offsetY = 2;
        }
        if (type == "LEG0")
        {
            offsetX = -1.9;
            offsetY = 12;
        }
        if (type == "LEG1")
        {
            offsetX = 1.9;
            offsetY = 12;
        }

        float x0 = offsetX + x - scale;
        float x1 = offsetX + x + width + scale;
        float y0 = -(offsetY + y) - scale;
        float y1 = -(offsetY + y) + height + scale;
        float z0 = offsetZ + z - scale;
        float z1 = offsetZ + z + depth + scale;

        if (mirrored) std::swap(x0, x1);

        glTexCoord2f(mFront.u0, mFront.v1); glVertex3f(x0, y0, z1);
        glTexCoord2f(mFront.u1, mFront.v1); glVertex3f(x1, y0, z1);
        glTexCoord2f(mFront.u1, mFront.v0); glVertex3f(x1, y1, z1);
        glTexCoord2f(mFront.u0, mFront.v0); glVertex3f(x0, y1, z1);

        glTexCoord2f(mBack.u1, mBack.v1); glVertex3f(x0, y0, z0);
        glTexCoord2f(mBack.u1, mBack.v0); glVertex3f(x0, y1, z0);
        glTexCoord2f(mBack.u0, mBack.v0); glVertex3f(x1, y1, z0);
        glTexCoord2f(mBack.u0, mBack.v1); glVertex3f(x1, y0, z0);

        glTexCoord2f(mTop.u0, mTop.v0); glVertex3f(x0, y1, z0);
        glTexCoord2f(mTop.u0, mTop.v1); glVertex3f(x0, y1, z1);
        glTexCoord2f(mTop.u1, mTop.v1); glVertex3f(x1, y1, z1);
        glTexCoord2f(mTop.u1, mTop.v0); glVertex3f(x1, y1, z0);

        glTexCoord2f(mBottom.u0, mirroredBottom ? mBottom.v0 : mBottom.v1); glVertex3f(x0, y0, z0);
        glTexCoord2f(mBottom.u1, mirroredBottom ? mBottom.v0 : mBottom.v1); glVertex3f(x1, y0, z0);
        glTexCoord2f(mBottom.u1, mirroredBottom ? mBottom.v1 : mBottom.v0); glVertex3f(x1, y0, z1);
        glTexCoord2f(mBottom.u0, mirroredBottom ? mBottom.v1 : mBottom.v0); glVertex3f(x0, y0, z1);

        glTexCoord2f(mRight.u1, mRight.v1); glVertex3f(x1, y0, z0);
        glTexCoord2f(mRight.u1, mRight.v0); glVertex3f(x1, y1, z0);
        glTexCoord2f(mRight.u0, mRight.v0); glVertex3f(x1, y1, z1);
        glTexCoord2f(mRight.u0, mRight.v1); glVertex3f(x1, y0, z1);

        glTexCoord2f(mLeft.u0, mLeft.v1); glVertex3f(x0, y0, z0);
        glTexCoord2f(mLeft.u1, mLeft.v1); glVertex3f(x0, y0, z1);
        glTexCoord2f(mLeft.u1, mLeft.v0); glVertex3f(x0, y1, z1);
        glTexCoord2f(mLeft.u0, mLeft.v0); glVertex3f(x0, y1, z0);

        glEnd();

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    }

    static void setMirroredBottom(bool value)
    {
        mirroredBottom = value;
    }

    static void setTextureSize(int width, int height)
    {
        textureWidth = width;
        textureHeight = height;
    }

private:
    UVRect mFront, mBack, mTop, mBottom, mRight, mLeft;

    void calculateUVs()
    {
        // Normalize variables
        float u0 = u / textureWidth;
        float v0 = v / textureHeight;
        float uvWidth = width / textureWidth;
        float uvHeight = height / textureHeight;
        float uvDepthX = depth / textureWidth;
        float uvDepthY = depth / textureHeight;

        mTop = { u0 + uvDepthX, v0, u0 + uvWidth + uvDepthX, v0 + uvDepthY };
        mBottom = { mTop.u1, mTop.v0, mTop.u1 + uvWidth, mTop.v1 };
        mLeft = { u0, mTop.v1, mTop.u0, v0 + uvDepthY + uvHeight };
        mFront = { mTop.u0, mTop.v1, mTop.u1, mLeft.v1 };
        mRight = { mTop.u1, mTop.v1, mTop.u1 + uvDepthX, mLeft.v1 };
        mBack = { mRight.u1, mTop.v1, mRight.u1 + uvWidth, mLeft.v1 };
    }
};