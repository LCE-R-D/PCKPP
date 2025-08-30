#include "UI/MenuFunctions.h"
#include "UI/SkinBox.h"
#include "UI/Tree/TreeFunctions.h"
#include "UI/UIImGui.h"
#include "Util/Util.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <sstream>

// Globals
static Texture gSkinTexture{}, gSkinPreviewTex{}, gSkinPreviewFBO{};
static GLuint gSkinPreviewDepth = 0;
float gRotationX = 0.0f;
float gRotationY = 0.0f;
float gPanX = 0.0f;
float gPanY = 0.0f;
float gZoom = 35.0f;

enum SKIN_ANIM
{
    STATIONARY_ARMS = 1 << 0,
    ZOMBIE_ARMS = 1 << 1,
    STATIONARY_LEGS = 1 << 2,
    BAD_SANTA_IDLE = 1 << 3,
    UNKNOWN_EFFECT = 1 << 4,
    SYNCHRONIZED_LEGS = 1 << 5,
    SYNCHRONIZED_ARMS = 1 << 6,
    STATUE_OF_LIBERTY = 1 << 7,
    HIDE_ARMOR = 1 << 8,
    FIRST_PERSON_BOBBING_DISABLED = 1 << 9,
    HIDE_HEAD = 1 << 10,
    HIDE_RIGHT_ARM = 1 << 11,
    HIDE_LEFT_ARM = 1 << 12,
    HIDE_BODY = 1 << 13,
    HIDE_RIGHT_LEG = 1 << 14,
    HIDE_LEFT_LEG = 1 << 15,
    HIDE_HAT = 1 << 16,
    BACKWARDS_CROUCH = 1 << 17, // This is what it's called on bedrock lol
    MODERN_WIDE_FORMAT = 1 << 18,
    SLIM_FORMAT = 1 << 19,
    HIDE_LEFT_SLEEVE = 1 << 20,
    HIDE_RIGHT_SLEEVE = 1 << 21,
    HIDE_LEFT_PANT = 1 << 22,
    HIDE_RIGHT_PANT = 1 << 23,
    HIDE_JACKET = 1 << 24,
    ALLOW_HEAD_ARMOR = 1 << 25, // these flags handle allowing a piece of armor to render after its parent part was hidden
    ALLOW_RIGHT_ARM_ARMOR = 1 << 26,
    ALLOW_LEFT_ARM_ARMOR = 1 << 27,
    ALLOW_CHESTPLATE = 1 << 28,
    ALLOW_RIGHT_LEGGING = 1 << 29,
    ALLOW_LEFT_LEGGING = 1 << 30,
    DINNERBONE_RENDERING = 1u << 31
};

void glPerspective(float fovY, float aspect, float zNear, float zFar)
{
    float fH = tanf(fovY * 0.5f * (M_PI / 180.0f)) * zNear;
    float fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

uint32_t ANIM = 0;
bool modernFormat = false;
bool slimFormat = false;
std::vector<SkinBox> boxes{};
std::vector<SkinBox> defaultBoxes{};

void SetUpSkinPreview(PCKAssetFile& file)
{
    GraphicsOpenGL* graphics = gApp->GetGraphics();
    graphics->DeleteTexture(gSkinTexture);
    gSkinTexture = graphics->LoadTextureFromMemory(file.getData().data(), file.getFileSize());

    if (gSkinPreviewTex.id == 0) glGenTextures(1, &gSkinPreviewTex.id);
    if (gSkinPreviewFBO.id == 0) glGenFramebuffers(1, &gSkinPreviewFBO.id);
    if (gSkinPreviewDepth == 0) glGenRenderbuffers(1, &gSkinPreviewDepth);

    bool ANIM_found = false;

    boxes.clear();
    defaultBoxes.clear();

    for (const PCKAssetFile::Property& property : file.getProperties())
    {
        std::wstring value(property.second.begin(), property.second.end()); // convert UTF-16

        if (property.first == "ANIM")
        {
            long animValue = std::wcstol(value.c_str(), nullptr, 0); // parse number

            ANIM = static_cast<uint32_t>(animValue);

            ANIM_found = true;
        }
        else if (property.first == "BOX")
        {
            SkinBox box{};
            box.parse(value);
            boxes.push_back(box);
        }
    }

    std::cout << boxes.size() << std::endl;

    if (!ANIM_found)
        ANIM = 0;

    slimFormat = ANIM & SLIM_FORMAT;
    modernFormat = (ANIM & MODERN_WIDE_FORMAT) || slimFormat;

    // positions/offsets are work in progress :3
    // heads seem to have weird offsets??? will work on this at a later date
    SkinBox head("HEAD_DEFAULT", - 4, -8, -4, 8, 8, 8, 0, 0);
    SkinBox hat = SkinBox::CreateLayer(head, 32, 0, 0.5f);

    SkinBox body("BODY", - 4, -2, -2, 8, 12, 4, 16, 16);
    SkinBox jacket = SkinBox::CreateLayer(body, 16, 32, 0.5f);
    SkinBox cape("CAPE", -8, -4, -6, 10, 16, 1, 16, 16);

    SkinBox arm0("ARM0", slimFormat ? -2 : -3, -2, -2, slimFormat ? 3 : 4, 12, 4, 40, 16);
    SkinBox sleeve0 = SkinBox::CreateLayer(arm0, 40, 32, 0.5f);

    SkinBox arm1("ARM1", -1, -2, -2, arm0.width, 12, 4, modernFormat ? 32 : 40, modernFormat ? 48 : 16, 0, !modernFormat);
    SkinBox sleeve1 = SkinBox::CreateLayer(arm1, 48, 48, 0.5f);

    SkinBox leg0("LEG0", -2, 0, -2, 4, 12, 4, 0, 16);
    SkinBox pant0 = SkinBox::CreateLayer(leg0, 0, 32, 0.5f);

    SkinBox leg1("LEG1", -2, 0, -2, 4, 12, 4, modernFormat ? 16 : 0, modernFormat ? 48 : 16, 0, arm1.mirrored);
    SkinBox pant1 = SkinBox::CreateLayer(leg1, 0, 48, 0.5f);

    if (!(ANIM & HIDE_HAT)) defaultBoxes.push_back(hat);
    if (!(ANIM & HIDE_HEAD)) defaultBoxes.push_back(head);
    if (!(ANIM & HIDE_BODY)) defaultBoxes.push_back(body);
    if (!(ANIM & HIDE_RIGHT_ARM)) defaultBoxes.push_back(arm0);
    if (!(ANIM & HIDE_LEFT_ARM)) defaultBoxes.push_back(arm1);
    if (!(ANIM & HIDE_RIGHT_LEG)) defaultBoxes.push_back(leg0);
    if (!(ANIM & HIDE_LEFT_LEG)) defaultBoxes.push_back(leg1);

    if (modernFormat)
    {
        if (!(ANIM & HIDE_JACKET)) defaultBoxes.push_back(jacket);
        if (!(ANIM & HIDE_RIGHT_SLEEVE)) defaultBoxes.push_back(sleeve0);
        if (!(ANIM & HIDE_LEFT_SLEEVE)) defaultBoxes.push_back(sleeve1);
        if (!(ANIM & HIDE_RIGHT_PANT)) defaultBoxes.push_back(pant0);
        if (!(ANIM & HIDE_LEFT_PANT)) defaultBoxes.push_back(pant1);
    }

    SkinBox::setTextureSize(64, modernFormat ? 64 : 32);
    SkinBox::setMirroredBottom(modernFormat);
}

void PreviewSkin(PCKAssetFile& file, bool reset)
{
    // Load texture from file data
    if (gSkinTexture.id == 0 || reset)
    {
        SetUpSkinPreview(file);
    }

    ImGuiIO& io = ImGui::GetIO();
    if (ImGui::IsItemActive())
    {
        gRotationY += io.MouseDelta.x * 0.25f;
        gRotationX += io.MouseDelta.y * 0.25f;
    }

    float previewWidth = ImGui::GetContentRegionAvail().x * 0.75f;
    float previewHeight = ImGui::GetContentRegionAvail().y;

    // Setup OpenGL state
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.01f);

    // Bind and setup FBO
    glBindFramebuffer(GL_FRAMEBUFFER, gSkinPreviewFBO.id);
    glBindTexture(GL_TEXTURE_2D, gSkinPreviewTex.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, previewWidth, previewHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gSkinPreviewTex.id, 0);

    // Depth buffer for FBO
    glBindRenderbuffer(GL_RENDERBUFFER, gSkinPreviewDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, previewWidth, previewHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gSkinPreviewDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "FBO not complete!\n");

    glViewport(0, 0, previewWidth, previewHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glPerspective(45.0f, previewWidth / previewHeight, 0.1f, 1000.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -gZoom);
    glRotatef(gRotationX, 1, 0, 0);
    glRotatef(gRotationY, 0, 1, 0);

    // Bind skin texture
    glBindTexture(GL_TEXTURE_2D, gSkinTexture.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    // Draw boxes
    for (const SkinBox& box : defaultBoxes)
    {
        box.Draw();
    }

    for (const SkinBox& box : boxes)
    {
        //box.Draw();
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_ALPHA_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ImGui::Image((ImTextureID)(intptr_t)gSkinPreviewTex.id, ImVec2(previewWidth, previewHeight), ImVec2(0, 1), ImVec2(1, 0));

    // Zoom with mouse wheel
    if (ImGui::IsItemHovered() && io.MouseWheel != 0.0f)
    {
        gZoom -= io.MouseWheel * 2.0f; // adjust scroll speed here
    }

    // Side panel
    ImGui::SetCursorPos(ImVec2(previewWidth, ImGui::GetFrameHeight()));
    if (ImGui::BeginChild("SkinEditorPanel", ImGui::GetContentRegionAvail(), 0, ImGuiWindowFlags_NoTitleBar))
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        float availableX = ImGui::GetContentRegionAvail().x;
        float availableY = ImGui::GetContentRegionAvail().y;

        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowBounds = ImVec2(windowPos.x + availableX, windowPos.y + availableY);

        draw_list->AddRectFilled(windowPos, windowBounds, IM_COL32(60, 60, 60, 255)); // dark gray

        ImGui::Image((ImTextureID)(intptr_t)gSkinTexture.id, { availableX, modernFormat ? availableX : availableX / 2});

        ImGui::EndChild();
    }
}