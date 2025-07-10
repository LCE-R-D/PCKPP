// PCK++ by May/MattNL :3

#include "UI/UI.h"
#include <SDL3/SDL_main.h>
#include <filesystem>

int main(int argc, char* argv[])
{
	if (!GraphicsSetup()) {
		return 1;
	}

	if (argc > 1)
	{
		std::string filepath = argv[1];

		std::filesystem::current_path(SDL_GetBasePath()); // set working directory to executable path

		if (std::ifstream(filepath))
			OpenPCKFile(filepath);
	}

	SDL_Window* window = GetWindow();
	UISetup();

	bool running = true;

	ImGuiIO& io = ImGui::GetIO();

	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT)
				running = false;
		}

		if (shouldClose())
			running = false;

		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		HandleInput();
		HandleMenuBar();
		HandleFileTree();

		ImGui::Render();
		int display_w, display_h;
		SDL_GetWindowSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}

	UICleanup();
	GraphicsCleanup();
	return 0;
}