#include "Graphics.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture LoadTextureFromMemory(const void* data, size_t size)
{
	int width, height, channels;
	unsigned char* pixels = stbi_load_from_memory(
		reinterpret_cast<const unsigned char*>(data),
		static_cast<int>(size),
		&width,
		&height,
		&channels,
		4 // force RGBA
	);

	if (!pixels) {
		SDL_Log("Failed to load image: %s", stbi_failure_reason());
		return {};
	}

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA,
		width,
		height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		pixels);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(pixels);
	return { tex, width, height };
}

// the texture specifically for the preview window
Texture g_previewTexture = {};

void PreviewImage(const PCKAssetFile& file)
{
	// If there's already a texture, delete it
	if (g_previewTexture.id != 0)
	{
		glDeleteTextures(1, &g_previewTexture.id);
		g_previewTexture.id = 0;
	}

	// Load new texture from file data
	g_previewTexture = LoadTextureFromMemory(file.getData().data(), file.getFileSize());

	if (g_previewTexture.id == 0) return; // Failed to load

	ImGui::Begin("Preview", nullptr, 
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoBringToFrontOnFocus);

	ImGui::SetNextWindowPos(ImVec2(512, 512));

	ImVec2 availSize = ImGui::GetContentRegionAvail();
	float windowRatio = availSize.x / availSize.y;
	float imageRatio = (float)g_previewTexture.width / (float)g_previewTexture.height;

	ImVec2 imageSize;
	if (windowRatio > imageRatio)
	{
		// Window is wider, limit by height
		imageSize.y = availSize.y;
		imageSize.x = imageRatio * availSize.y;
	}
	else
	{
		// Window is taller, limit by width
		imageSize.x = availSize.x;
		imageSize.y = availSize.x / imageRatio;
	}

	ImGui::Image((ImTextureID)(intptr_t)g_previewTexture.id, imageSize);
	ImGui::End();
}