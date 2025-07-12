// PCK++ by May/MattNL :3

#include "Application/Application.h"
#include "UI/UI.h"
#include "Graphics/Graphics.h"

#include <SDL3/SDL_main.h>

int main(int argc, char* argv[]) {
    if (!GraphicsSetup())
        return 1;

    Application app;
    gApp = &app;

    if (!gApp->Init(argc, argv))
        return 1;

    SDL_Window* window = GetWindow();
    UISetup();

    bool running = true;
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

        gApp->Update();

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
    gApp->Shutdown();

    return 0;
}