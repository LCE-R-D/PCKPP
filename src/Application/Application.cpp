#include "Application.h"
#include <fstream>
#include <iostream>

Application* gApp = nullptr;

bool Application::Init(int argc, char* argv[]) {
    std::filesystem::current_path(SDL_GetBasePath());

    if (argc > 1) {
        LoadPCKFile(argv[1]);
    }

    initialized = true;
    return true;
}

PCKFile* Application::CurrentPCKFile()
{
    SDL_Log("%d", bool(mCurrentPCKFile));
    return mCurrentPCKFile;
}

void Application::LoadPCKFile(const std::string& filepath)
{
    try
    {
        mCurrentPCKFile = new PCKFile();
        mCurrentPCKFile->Read(filepath);
    }
    catch (...)
    {
        std::cerr << "Failed to load PCK file: " << filepath << "\n";
    }
}

void Application::Update()
{

}

void Application::Shutdown()
{

}