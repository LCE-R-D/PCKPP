#include <imgui.h>
#include <SDL3/SDL.h>
#include <cstdio>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_opengl2.h>
#include <glad/glad.h>
#include <vector>

SDL_Window* window = nullptr;
SDL_GLContext context = nullptr;
bool shouldClose = false;

void HandleMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                void* data = nullptr;
                SDL_DialogFileCallback callback = nullptr;
                SDL_DialogFileFilter filters[] = {"Minecraft LCE DLC Files", "pck"};

                SDL_ShowOpenFileDialog(callback, data, window, filters, SDL_arraysize(filters), "", false);

//                nfdu8char_t* outPath = nullptr;
//                nfdu8filteritem_t filters[] = { { "Minecraft LCE DLC Files", "pck" } };
//                nfdopendialogu8args_t args = { 0 };
//                args.filterList = filters;
//                args.filterCount = sizeof(filters) / sizeof(filters[0]);
//                NFD_GetNativeWindowFromGLFWWindow(window, &args.parentWindow);

//                nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
//                if (result == NFD_OKAY) {
//                    printf("Selected file: %s\n", outPath);
//                    NFD_FreePathU8(outPath);
//                }
//                else if (result == NFD_CANCEL) {
//                    printf("User cancelled.");
//                }
//                else {
//                    printf("Error: %s\n", NFD_GetError());
//                }
            }
            ImGui::MenuItem("Save", "Ctrl+S");
            if (ImGui::MenuItem("Quit")) {
                shouldClose = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);
    ImGui::Begin("File List", nullptr,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("File Tree");
    ImGui::End();
}

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO))
        return 1;

    window = SDL_CreateWindow("PCK++", 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window)
        return 1;

    context = SDL_GL_CreateContext(window);
    if (!SDL_GL_SetSwapInterval(1)) {
        return 1;
    }

    if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2))
        return 1;
    if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1))
        return 1;

    if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY))
        return 1;

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
        return 1;
    }

    if (!SDL_GL_MakeCurrent(window, context)) {
        return 1;
    }

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplOpenGL2_Init();
    ImGui_ImplSDL3_InitForOpenGL(window, context);

    while (!shouldClose) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    shouldClose = true;
                    break;
            }
        }

        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        HandleMenuBar();

        ImGui::Render();
        int display_w, display_h;
        SDL_GetWindowSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}