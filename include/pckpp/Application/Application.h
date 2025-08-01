#pragma once

#include <iostream>
#include <map>
#include <pckpp/Graphics/GraphicsBase.h>
#include <pckpp/Platform/PlatformBase.h>
#include <pckpp/Program/ProgramInstance.h>
#include <pckpp/UI/UIBase.h>

template<typename TPlatform, typename TGraphics, typename TUI>
class Application {
public:
    static_assert(std::is_base_of<PlatformBase, TPlatform>::value, "TPlatform must derive from PlatformBase");
    static_assert(std::is_base_of<GraphicsBase, TGraphics>::value, "TGraphics must derive from GraphicsBase");
    static_assert(std::is_base_of<UIBase, TUI>::value, "TUI must derive from UIBase");

    Application() = default;
    ~Application() = default;

    // Gets the Platform Backend
    PlatformBackend* GetPlatformBackend();

    // Gets the Renderer Backend
    RendererBackend* GetRendererBackend();

    // Sets the platform backend for the application and UI to use
    void SetPlatformBackend(std::unique_ptr<PlatformBackend> backend);

    // Sets the renderer backend for the application and UI to use
    void SetRendererBackend(std::unique_ptr<RendererBackend> backend);

    // Initialize core app; pass argc/argv if needed for CLI args
    bool Init(int argc, char* argv[]);

    // Cleanup any core resources
    void Shutdown();

    // Application Update
    void Update();

    ProgramInstance* GetInstance() const;

    // Gets Platform framework from the application
    TPlatform* GetPlatform() const;

    // Gets Graphics framework  from the application
    TGraphics* GetGraphics() const;

    // Gets UI framework from the appliction
    TUI* GetUI() const;

    const Texture& GetFileIcon(PCKAssetFile::Type type) const;
    void SetFileIcon(PCKAssetFile::Type type, const Texture& texture);

    const Texture& GetFolderIcon() const;
    void SetFolderIcon(const Texture& texture);

    const Texture& GetPreviewTexture() const;
    void SetPreviewTexture(const Texture& texture);

private:
    std::unique_ptr<ProgramInstance> mInstance{new ProgramInstance()};
    std::unique_ptr<TPlatform> mPlatform{};
    std::unique_ptr<TGraphics> mGraphics{};
    std::unique_ptr<TUI> mUI{};
    std::unique_ptr<PlatformBackend> mPlatformBackend{};
    std::unique_ptr<RendererBackend> mRendererBackend{};
    bool initialized{ false };

    // Icons for File Tree Nodes, indexed via PCK filetypes
    std::map<PCKAssetFile::Type, Texture> mFileIcons{};
    // Texture for folders
    Texture mFolderIcon{};
    // Current texture for the preview window
    Texture mPreviewTexture{};
};

// Setup your custom application stuff below :3
#include "../Platform/PlatformSDL.h"
#include "../Graphics/GraphicsOpenGL.h"
#include "../UI/UIImGui.h"

using DefaultGraphics = GraphicsOpenGL;
using DefaultUI = UIImGui;
using DefaultPlatform = PlatformSDL;

using DefaultApp = Application<DefaultPlatform, DefaultGraphics, DefaultUI>;

extern DefaultApp* gApp;