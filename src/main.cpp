#include <imgui.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <nfd_glfw3.h>
#include <stdio.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

GLFWwindow* window = nullptr;

void HandleMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                nfdu8char_t* outPath = nullptr;
                nfdu8filteritem_t filters[] = { { "Minecraft LCE DLC Files", "pck" } };
                nfdopendialogu8args_t args = { 0 };
                args.filterList = filters;
                args.filterCount = sizeof(filters) / sizeof(filters[0]);
                NFD_GetNativeWindowFromGLFWWindow(window, &args.parentWindow);

                nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
                if (result == NFD_OKAY) {
                    printf("Selected file: %s\n", outPath);
                    NFD_FreePathU8(outPath);
                }
                else if (result == NFD_CANCEL) {
                    printf("User cancelled.");
                }
                else {
                    printf("Error: %s\n", NFD_GetError());
                }
            }
            ImGui::MenuItem("Save", "Ctrl+S");
            if (ImGui::MenuItem("Quit")) {
                glfwSetWindowShouldClose(window, true);
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
    if (!glfwInit())
        return 1;

    window = glfwCreateWindow(1280, 720, "PCK++", nullptr, nullptr);
    if (!window)
        return 1;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        HandleMenuBar();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}