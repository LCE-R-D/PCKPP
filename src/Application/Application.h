#pragma once

#include <string>
#include <memory>
#include "../PCK/PCKFile.h"
#include "../Graphics/GraphicsBase.h"
#include "../Platform/PlatformBase.h"
#include "../Backends/PlatformBackend.h"
#include "../Backends/RendererBackend.h"
#include "../UI/UIBase.h"

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

    // Get current PCK file; TODO: make this support multiple pcks
    PCKFile* CurrentPCKFile();

    // Loads PCK File from path
    void LoadPCKFile(const std::string& filepath);

    // Application Update
    void Update();

    // Gets Platform framework from the application
    TPlatform* GetPlatform() const;

    // Gets Graphics framework  from the application
    TGraphics* GetGraphics() const;

    // Gets UI framework from the appliction
    TUI* GetUI() const;

private:
    std::unique_ptr<PCKFile> mCurrentPCKFile;
    std::unique_ptr<TPlatform> mPlatform;
    std::unique_ptr<TGraphics> mGraphics;
    std::unique_ptr<TUI> mUI;
    std::unique_ptr<PlatformBackend> mPlatformBackend;
    std::unique_ptr<RendererBackend> mRendererBackend;
    bool initialized{ false };
};

#include "../Platform/PlatformSDL.h"
#include "../Graphics/GraphicsOpenGL.h"
#include "../UI/UIImGui.h"

using DefaultGraphics = GraphicsOpenGL;
using DefaultUI = UIImGui;
using DefaultPlatform = PlatformSDL;

using DefaultApp = Application<DefaultPlatform, DefaultGraphics, DefaultUI>;

extern DefaultApp* gApp;