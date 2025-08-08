#include <pckpp/UI/UIImGui.h>
#include <pckpp/UI/MenuFunctions.h>
#include <pckpp/UI/Tree/TreeFunctions.h>
#include <pckpp/Util.h>

static GLuint gSkinPreviewFBO = 0, gSkinPreviewTex = 0;
constexpr int gSkinPreviewSize = 512;

// Globals
float gRotationX = 0.0f;
float gRotationY = 0.0f;

void PreviewSkin(PCKAssetFile& file)
{
	float width = ImGui::GetContentRegionAvail().x * 0.75;
	float height = ImGui::GetContentRegionAvail().y;

	glGenFramebuffers(1, &gSkinPreviewFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, gSkinPreviewFBO);

	glGenTextures(1, &gSkinPreviewTex);
	glBindTexture(GL_TEXTURE_2D, gSkinPreviewTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gSkinPreviewTex, 0);

	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		fprintf(stderr, "FBO not complete!\n");

	glBindFramebuffer(GL_FRAMEBUFFER, gSkinPreviewFBO);

	// Only rotate if the invisible button (canvas) is active (clicked and held)
	if (ImGui::IsItemActive() && ImGui::GetIO().MouseDelta.x != 0.0f && ImGui::GetIO().MouseDelta.y != 0.0f)
	{
		gRotationX += ImGui::GetIO().MouseDelta.y * 0.25f;
		gRotationY += ImGui::GetIO().MouseDelta.x * 0.25f;
	}

	glViewport(0, 0, width, height);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(gRotationX, 1, 0, 0);
	glRotatef(gRotationY, 0, 1, 0);
	glTranslatef(width / 2, height / 2, 0);

	float triSize = 200.0f;
	float half = triSize / 2.0f;

	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(-half, -half, 0.0f);  // bottom-left
	glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(half, -half, 0.0f);  // bottom-right
	glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(0.0f, half, 0.0f);  // top-center
	glEnd();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Finally display on UI
	ImGui::Image(
		reinterpret_cast<void*>((intptr_t)gSkinPreviewTex),
		ImVec2(width, height),
		ImVec2(0, 1), ImVec2(1, 0) // Flip Y
	);
}