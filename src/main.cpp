// PCK++ by May/MattNL :3

#include "Application/Application.h"
#include "Program/Program.h"

#include "Backends/ImGuiSDLPlatformBackend.hpp"
#include "Backends/ImGuiOpenGLRendererBackend.hpp"

int main(int argc, char* argv[]) {
	if (!gApp->Init(argc, argv))
		return 1;

	auto platform = gApp->GetPlatform();
	auto graphics = gApp->GetGraphics();
	auto ui = gApp->GetUI();

	auto platformBackend = std::make_unique<ImGuiSDLPlatformBackend>();
	auto rendererBackend = std::make_unique<ImGuiOpenGLRendererBackend>();

	if (!platform->Init("PCK++", 1280, 720))
		return 1;

	if (!graphics->Init())
		return 1;

	if (!ui->Init())
		return 1;

	gApp->SetPlatformBackend(std::move(platformBackend));
	gApp->SetRendererBackend(std::move(rendererBackend));

	if (!ui->InitBackends(platform->GetWindow(), nullptr))
		return 1;

	const auto& backend = gApp->GetPlatformBackend();

	// platforms implementions must have a custom GetWindow function
	SDL_Window* window = platform->GetWindow();

	ProgramSetup();

	while (!platform->ShouldClose()) {
		platform->PollEvents(backend);
		ui->NewFrame();
		graphics->NewFrame();
		gApp->Update();
		ui->Render();
		graphics->Render();
		SDL_GL_SwapWindow(window);
	}

	ui->Shutdown();
	graphics->Shutdown();
	platform->Shutdown();
	gApp->Shutdown();

	return 0;
}