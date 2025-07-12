#pragma once
#include <string>
#include <memory>
#include "../PCK/PCKFile.h"

class Application {
public:
    Application() = default;
    ~Application() = default;

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

private:
    PCKFile* mCurrentPCKFile{ nullptr };
    bool initialized{ false };
};

extern Application* gApp;