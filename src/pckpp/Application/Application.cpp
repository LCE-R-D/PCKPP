#include "Application.h"
#include "../Program/Program.h"
#include "../UI/Menu/MenuFunctions.h"
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
    mPlatform = std::make_unique<TPlatform>();
    mGraphics = std::make_unique<TGraphics>();
    mUI = std::make_unique<TUI>();
    mInstance = std::make_unique<ProgramInstance>();

    if (argc > 1) {
        OpenPCKFile(argv[1]);
    }

    initialized = true;
    return true;
}

template<typename TPlatform, typename TGraphics, typename TUI>
void Application<TPlatform, TGraphics, TUI>::Update() {
    HandleInput();
    HandleMenuBar();
    HandleFileTree();
}

template<typename TPlatform, typename TGraphics, typename TUI>
void Application<TPlatform, TGraphics, TUI>::Shutdown() {
    for (auto& [type, tex] : mFileIcons)
        if (tex.id != 0)
            mGraphics->DeleteTexture(tex);

    mFileIcons.clear();
}

template<typename TPlatform, typename TGraphics, typename TUI>
TPlatform* Application<TPlatform, TGraphics, TUI>::GetPlatform() const
{
    return mPlatform.get();
}

template<typename TPlatform, typename TGraphics, typename TUI>
ProgramInstance* Application<TPlatform, TGraphics, TUI>::GetInstance() const
{
    return mInstance.get();
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

template<typename TPlatform, typename TGraphics, typename TUI>
const Texture& Application<TPlatform, TGraphics, TUI>::GetFileIcon(PCKAssetFile::Type type) const {
    static Texture empty{};
    auto it = mFileIcons.find(type);
    return it != mFileIcons.end() ? it->second : empty;
}

template<typename TPlatform, typename TGraphics, typename TUI>
void Application<TPlatform, TGraphics, TUI>::SetFileIcon(PCKAssetFile::Type type, const Texture& texture) {
    mFileIcons[type] = texture;
}

template<typename TPlatform, typename TGraphics, typename TUI>
const Texture& Application<TPlatform, TGraphics, TUI>::GetFolderIcon() const {
    return mFolderIcon;
}

template<typename TPlatform, typename TGraphics, typename TUI>
void Application<TPlatform, TGraphics, TUI>::SetFolderIcon(const Texture& texture) {
    mFolderIcon = texture;
}

template<typename TPlatform, typename TGraphics, typename TUI>
const Texture& Application<TPlatform, TGraphics, TUI>::GetPreviewTexture() const {
    return mPreviewTexture;
}

template<typename TPlatform, typename TGraphics, typename TUI>
void Application<TPlatform, TGraphics, TUI>::SetPreviewTexture(const Texture& texture) {
    mPreviewTexture = texture;
}