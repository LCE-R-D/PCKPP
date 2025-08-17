#include <pckpp/UI/MenuFunctions.h>
#include <pckpp/UI/SkinBox.h>
#include <pckpp/UI/Tree/TreeFunctions.h>
#include <pckpp/UI/UIImGui.h>
#include <pckpp/Util.h>
#define _USE_MATH_DEFINES
#include <math.h>

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

void PreviewSkin(PCKAssetFile& file, bool reset)
{
    std::vector<SkinBox> boxes{};

    // Load texture from file data
    if (gSkinTexture.id == 0 || reset)
    {
        GraphicsOpenGL* graphics = gApp->GetGraphics();
        graphics->DeleteTexture(gSkinTexture);
        gSkinTexture = graphics->LoadTextureFromMemory(file.getData().data(), file.getFileSize());

        if (gSkinPreviewTex.id == 0) glGenTextures(1, &gSkinPreviewTex.id);
        if (gSkinPreviewFBO.id == 0) glGenFramebuffers(1, &gSkinPreviewFBO.id);
        if (gSkinPreviewDepth == 0) glGenRenderbuffers(1, &gSkinPreviewDepth);

        bool ANIM_found = false;

        for (const PCKAssetFile::Property& property : file.getProperties())
        {
            if (property.first == "ANIM")
            {
                std::wstring ws(property.second.begin(), property.second.end()); // convert UTF-16
                long animValue = std::wcstol(ws.c_str(), nullptr, 0); // parse number

                ANIM = static_cast<uint32_t>(animValue);

                ANIM_found = true;
                break;
            }
        }

        if (!ANIM_found)
            ANIM = 0;

        slimFormat = ANIM & SLIM_FORMAT;
        modernFormat = (ANIM & MODERN_WIDE_FORMAT) || slimFormat;

        SkinBox::setTextureSize(64, modernFormat ? 64 : 32);
        SkinBox::setMirroredBottom(modernFormat);
    }

    // positions/offsets are work in progress :3
    SkinBox head(-8, -16, -8, 8, 8, 8, 0, 0);
    SkinBox hat = SkinBox::CreateLayer(head, 32, 0, 0.5f);

    SkinBox body(-8, -4, -6, 8, 12, 4, 16, 16);
    SkinBox jacket = SkinBox::CreateLayer(body, 16, 32, 0.5f);

    SkinBox arm0(slimFormat ? -11 : -12, -4, body.z, slimFormat ? 3 : 4, 12, 4, 40, 16);
    SkinBox sleeve0 = SkinBox::CreateLayer(arm0, 40, 32, 0.5f);

    SkinBox arm1(0, -4, body.z, arm0.width, 12, 4, modernFormat ? 32 : 40, modernFormat ? 48 : 16, 0, !modernFormat);
    SkinBox sleeve1 = SkinBox::CreateLayer(arm1, 48, 48, 0.5f);

    SkinBox leg0(-8, 8, body.z, 4, 12, 4, 0, 16);
    SkinBox pant0 = SkinBox::CreateLayer(leg0, 0, 32, 0.5f);

    SkinBox leg1(-4, 8, body.z, 4, 12, 4, modernFormat ? 16 : 0, modernFormat ? 48 : 16, 0, arm1.mirrored);
    SkinBox pant1 = SkinBox::CreateLayer(leg1, 0, 48, 0.5f);

    boxes.clear();

    if (!(ANIM & HIDE_HAT)) boxes.push_back(hat);
    if (!(ANIM & HIDE_HEAD)) boxes.push_back(head);
    if (!(ANIM & HIDE_BODY)) boxes.push_back(body);
    if (!(ANIM & HIDE_RIGHT_ARM)) boxes.push_back(arm0);
    if (!(ANIM & HIDE_LEFT_ARM)) boxes.push_back(arm1);
    if (!(ANIM & HIDE_RIGHT_LEG)) boxes.push_back(leg0);
    if (!(ANIM & HIDE_LEFT_LEG)) boxes.push_back(leg1);

    if (modernFormat)
    {
        if (!(ANIM & HIDE_JACKET)) boxes.push_back(jacket);
        if (!(ANIM & HIDE_RIGHT_SLEEVE)) boxes.push_back(sleeve0);
        if (!(ANIM & HIDE_LEFT_SLEEVE)) boxes.push_back(sleeve1);
        if (!(ANIM & HIDE_RIGHT_PANT)) boxes.push_back(pant0);
        if (!(ANIM & HIDE_LEFT_PANT)) boxes.push_back(pant1);
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
    for (const SkinBox& box : boxes)
    {
        box.Draw();
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