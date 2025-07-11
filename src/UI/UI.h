#pragma once

#include "../PCK/PCKFile.h"
#include "../Graphics/Graphics.h"
#include "Menu/MenuFunctions.h"
#include "../Math/Vec2Operators.h"
#include <sstream>
#include <filesystem>
#include <iomanip>
#include <map>

// Get the instance's current PCK File
PCKFile*& GetCurrentPCKFile();

// Setup UI related Vendors
void UISetup();

// Cleanup UI related vendors
void UICleanup();

// Reset PCK++ UI Data; optional filepath passing for when opening the file
void ResetUIData(const std::string& filePath = "");

// Handles the menu bar, functions are held in MenuFunctions.h/cpp
void HandleMenuBar();

// Handle input for the UI, like keystrokes
void HandleInput();

// Instead of writing a 1,000 success messages
void ShowSuccessMessage();

// Instead of writing a 1,000 cancelled messages
void ShowCancelledMessage();

// A helper function to cut cown on work required to open a message prompt box
static int ShowMessagePrompt(const char* title, const char* message, const SDL_MessageBoxButtonData* buttons, int numButtons);

// Basically a helper function to do Yes/No prompts using ShowMessagePromptBox
static bool ShowYesNoMessagePrompt(const char* title, const char* message);

// Handles the file tree, functions are held in TreeFunctions.h/cpp
void HandleFileTree();