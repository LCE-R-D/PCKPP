#pragma once

#include "../PCK/PCKFile.h"
#include "../Graphics/Graphics.h"
#include "Menu/MenuFunctions.h"
#include "../Math/Vec2Operators.h"

// Get the instance's current PCK File
PCKFile*& GetCurrentPCKFile();

// Get the instance's current ImGui IO stuff
ImGuiIO*& GetImGuiIO();

// Handle input for the UI, like keystrokes
void HandleInput();

// Render and Maintain Menu Bar
void HandleMenuBar();

// Render and Maintain File Tree
void HandleFileTree();

// Reset PCK++ UI Data; optional filepath passing for when opening the file
void ResetUIData(const std::string& filePath = "");

// Setup UI related Vendors
void UISetup();

// Cleanup UI related vendors
void UICleanup();

// A helper function to cut cown on work required
static int ShowMessagePrompt(const char* title, const char* message, const SDL_MessageBoxButtonData* buttons, int numButtons);

// Basically a helper function to do Yes/No prompts using ShowMessagePromptBox
static bool ShowYesNoMessagePrompt(const char* title, const char* message);