#include <pckpp/UI/MenuFunctions.h>
#include <pckpp/UI/SkinBox.h>
#include <pckpp/UI/Tree/TreeFunctions.h>
#include <pckpp/UI/UIImGui.h>
#include <pckpp/Util.h>
#define _USE_MATH_DEFINES
#include <math.h>

// Globals
static Texture gSkinTexture{}, gSkinPreviewTex{}, gSkinPreviewFBO{};
float gRotationX = 0.0f;
float gRotationY = 0.0f;

float gPanX = 0.0f;
float gPanY = 0.0f;
float gZoom = 25.0f;

void glPerspective(float fovY, float aspect, float zNear, float zFar)
{
	float fH = tanf(fovY * 0.5f * (M_PI / 180.0f)) * zNear;
	float fW = fH * aspect;
	glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

void PreviewSkin(PCKAssetFile& file, bool reset)
{
    // Load texture from the file data
    if (gSkinTexture.id == 0 || reset)
    {
        GraphicsOpenGL* graphics = gApp->GetGraphics();
        graphics->DeleteTexture(gSkinTexture);
        gSkinTexture = graphics->LoadTextureFromMemory(file.getData().data(), file.getFileSize());
        if(&gSkinPreviewTex)
            glGenTextures(1, &gSkinPreviewTex.id);
        if(&gSkinPreviewFBO)
            glGenFramebuffers(1, &gSkinPreviewFBO.id);
    }

    ImGuiIO& io = ImGui::GetIO();

    bool active = ImGui::IsItemActive();

    if (active)
    {
        gRotationY += io.MouseDelta.x * 0.25f; // yaw
        gRotationX += io.MouseDelta.y * 0.25f; // pitch
    }

    float width = ImGui::GetContentRegionAvail().x * 0.75f;
    float height = ImGui::GetContentRegionAvail().y;

    glBindFramebuffer(GL_FRAMEBUFFER, gSkinPreviewFBO.id);

    glBindTexture(GL_TEXTURE_2D, gSkinPreviewTex.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gSkinPreviewTex.id, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "FBO not complete!\n");

    // Draw scene
    glBindFramebuffer(GL_FRAMEBUFFER, gSkinPreviewFBO.id);
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Transparency
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glPerspective(45.0f, width / height, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Move camera back
    glTranslatef(0.0f, 0.0f, -gZoom); // use zoom here

    // Apply pitch and yaw
    glRotatef(gRotationX, 1, 0, 0);
    glRotatef(gRotationY, 0, 1, 0);

    // Bind skin texture
    glBindTexture(GL_TEXTURE_2D, gSkinTexture.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);

    Box head(gSkinTexture.width, gSkinTexture.height, 0, 0, 0, 8, 8, 8, 0, 0);
    //Box body(gSkinTexture.width, gSkinTexture.height, 0, 0, 0, 8, 12, 4, 16, 16);

    head.Draw();
    //body.Draw();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Display in ImGui
    ImGui::Image((ImTextureID)(intptr_t)gSkinPreviewTex.id, ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));

    if (ImGui::IsItemHovered() && io.MouseWheel != 0.0f)
    {
        gZoom -= io.MouseWheel * 0.5f; // adjust speed here
    }
}