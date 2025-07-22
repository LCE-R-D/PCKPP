#include "Application.h"
#include "../Program.h"
#include <fstream>
#include <iostream>

// just for the sake of making things a little clearer
DefaultApp* gApp = new DefaultApp();

template class Application<DefaultPlatform, DefaultGraphics, DefaultUI>;

template<typename TPlatform, typename TGraphics, typename TUI>
PlatformBackend* Application<TPlatform, TGraphics, TUI>::GetPlatformBackend()
{
    return mPlatformBackend.get();
}

template<typename TPlatform, typename TGraphics, typename TUI>
RendererBackend* Application<TPlatform, TGraphics, TUI>::GetRendererBackend()
{
    return mRendererBackend.get();
}

template<typename TPlatform, typename TGraphics, typename TUI>
void Application<TPlatform, TGraphics, TUI>::SetPlatformBackend(std::unique_ptr<PlatformBackend> backend) {
    mPlatformBackend = std::move(backend);
}

template<typename TPlatform, typename TGraphics, typename TUI>
void Application<TPlatform, TGraphics, TUI>::SetRendererBackend(std::unique_ptr<RendererBackend> backend) {
    mRendererBackend = std::move(backend);
}

template<typename TPlatform, typename TGraphics, typename TUI>
bool Application<TPlatform, TGraphics, TUI>::Init(int argc, char* argv[]) {
    std::filesystem::current_path(SDL_GetBasePath());

    if (argc > 1) {
        LoadPCKFile(argv[1]);
    }

    mPlatform = std::make_unique<TPlatform>();
    mGraphics = std::make_unique<TGraphics>();
    mUI = std::make_unique<TUI>();

    initialized = true;
    return true;
}

template<typename TPlatform, typename TGraphics, typename TUI>
PCKFile* Application<TPlatform, TGraphics, TUI>::CurrentPCKFile() {
    return mCurrentPCKFile.get();
}

template<typename TPlatform, typename TGraphics, typename TUI>
void Application<TPlatform, TGraphics, TUI>::LoadPCKFile(const std::string& filepath) {
    try {
        mCurrentPCKFile = std::make_unique<PCKFile>();
        mCurrentPCKFile->Read(filepath);
    }
    catch (...) {
        std::cerr << "Failed to load PCK file: " << filepath << "\n";
    }
}

template<typename TPlatform, typename TGraphics, typename TUI>
void Application<TPlatform, TGraphics, TUI>::Update() {
    HandleInput();
    HandleMenuBar();
    HandleFileTree();
}

template<typename TPlatform, typename TGraphics, typename TUI>
void Application<TPlatform, TGraphics, TUI>::Shutdown() {}

template<typename TPlatform, typename TGraphics, typename TUI>
TPlatform* Application<TPlatform, TGraphics, TUI>::GetPlatform() const
{
    return mPlatform.get();
}

template<typename TPlatform, typename TGraphics, typename TUI>
TGraphics* Application<TPlatform, TGraphics, TUI>::GetGraphics() const
{
    return mGraphics.get();
}

template<typename TPlatform, typename TGraphics, typename TUI>
TUI* Application<TPlatform, TGraphics, TUI>::GetUI() const
{
    return mUI.get();
}