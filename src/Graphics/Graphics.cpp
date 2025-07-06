#include "Graphics.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

SDL_Window* gWindow{ nullptr };
SDL_GLContext gContext{ nullptr };
bool gShouldClose{ false };

bool shouldClose()
{
	return gShouldClose;
}

SDL_Window* GetWindow()
{
	return gWindow;
}

Texture LoadTextureFromMemory(const void* data, size_t size, int glFilter)
{
	if (!data || size == 0) {
		SDL_Log("Empty buffer passed to LoadTextureFromMemory.");
		return {};
	}

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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilter);
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

	// only generate textures with mipmaps enabled
	if (glFilter == GL_LINEAR_MIPMAP_LINEAR ||
		glFilter == GL_NEAREST_MIPMAP_LINEAR ||
		glFilter == GL_LINEAR_MIPMAP_NEAREST ||
		glFilter == GL_NEAREST_MIPMAP_NEAREST)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(pixels);
	return { tex, width, height };
}

Texture LoadTextureFromFile(const std::string& path, int glFilter)
{
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if (!file)
	{
		SDL_Log("Failed to open image file: %s", path.c_str());
		return {};
	}

	std::size_t size = file.tellg();

	if (size <= 0) {
		SDL_Log("Empty buffer passed to LoadTextureFromFile.");
		return {};
	}

	file.seekg(0, std::ios::beg);

	std::vector<unsigned char> buffer(size);
	if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
	{
		SDL_Log("Failed to read image file: %s", path.c_str());
		return {};
	}

	SDL_Log("Loaded: %s", path.c_str());

	return LoadTextureFromMemory(buffer.data(), buffer.size(), glFilter);
}

bool GraphicsSetup()
{
	if (!SDL_Init(SDL_INIT_VIDEO))
		return false;

	gWindow = SDL_CreateWindow("PCK++", 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!gWindow)
		return false;

	gContext = SDL_GL_CreateContext(gWindow);
	if (!SDL_GL_SetSwapInterval(1)) {
		return false;
	}

	if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2))
		return false;
	if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1))
		return false;

	if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY))
		return false;

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
		return false;
	}

	if (!SDL_GL_MakeCurrent(gWindow, gContext)) {
		return false;
	}

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplOpenGL2_Init();
	ImGui_ImplSDL3_InitForOpenGL(gWindow, gContext);

	return true;
}

void GraphicsCleanup()
{
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyWindow(gWindow);
	SDL_Quit();
}