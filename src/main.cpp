// PCKPP by May/MattNL :3

#include "Graphics/Graphics.h"
#include "UI/UI.h"

int main() 
{
	if (!GraphicsSetup()) {
		return 1;
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

		// make sure to pass false or else it will trigger multiple times
		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_O, false))
		{
			OpenPCKFile(window);
		}

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