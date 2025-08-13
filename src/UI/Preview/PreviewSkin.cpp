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
        if (&gSkinPreviewTex)
            glGenTextures(1, &gSkinPreviewTex.id);
        if (&gSkinPreviewFBO)
            glGenFramebuffers(1, &gSkinPreviewFBO.id);
    }

    ImGuiIO& io = ImGui::GetIO();

    bool active = ImGui::IsItemActive();

    if (active)
    {
        gRotationY += io.MouseDelta.x * 0.25f; // yaw
        gRotationX += io.MouseDelta.y * 0.25f; // pitch
    }

    float previewWidth = ImGui::GetContentRegionAvail().x * 0.75f;
    float previewHeight = ImGui::GetContentRegionAvail().y;

    // Bind and setup FBO and texture
    glBindFramebuffer(GL_FRAMEBUFFER, gSkinPreviewFBO.id);

    glBindTexture(GL_TEXTURE_2D, gSkinPreviewTex.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, previewWidth, previewHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gSkinPreviewTex.id, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "FBO not complete!\n");

    // Set viewport and clear buffers
    glViewport(0, 0, previewWidth, previewHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Transparent background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glPerspective(45.0f, (float)previewWidth / (float)previewHeight, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, -gZoom);
    glRotatef(gRotationX, 1, 0, 0);
    glRotatef(gRotationY, 0, 1, 0);

    glBindTexture(GL_TEXTURE_2D, gSkinTexture.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    Box head(gSkinTexture.width, gSkinTexture.height, -8, -16, -8, 8, 8, 8, 0, 0);
    //Box body(gSkinTexture.width, gSkinTexture.height, -8, 0, -4, 8, 12, 4, 16, 16);

    head.Draw();

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    Box hat(gSkinTexture.width, gSkinTexture.height, -8, -16, -8, 8, 8, 8, 32, 0, 0, false, 0.5f); // assuming alpha 0.5 for transparency

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    hat.Draw();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Display in ImGui
    ImGui::Image((ImTextureID)(intptr_t)gSkinPreviewTex.id, ImVec2(previewWidth, previewHeight), ImVec2(0, 1), ImVec2(1, 0));

    if (ImGui::IsItemHovered() && io.MouseWheel != 0.0f)
    {
        gZoom -= io.MouseWheel * 2.0f; // adjust scroll speed here
    }

    ImGui::SetCursorPos(ImVec2(previewWidth, ImGui::GetFrameHeight()));
    if (ImGui::BeginChild("SkinEditorPanel", { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y }, 0, ImGuiWindowFlags_NoTitleBar))
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        float availableSpaceX = ImGui::GetContentRegionAvail().x;
        float availableSpaceY = ImGui::GetContentRegionAvail().y;

        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowBounds = ImVec2(windowPos.x + availableSpaceX, windowPos.y + availableSpaceY);

        draw_list->AddRectFilled(windowPos, windowBounds, IM_COL32(60, 60, 60, 255)); // dark gray

        ImGui::Image((ImTextureID)(intptr_t)gSkinTexture.id, { availableSpaceX, availableSpaceX });

        ImGui::EndChild();
    }
}