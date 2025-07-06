#pragma once

#include "../PCK/PCKFile.h"
#include "../Graphics/Graphics.h"
#include "Menu/MenuFunctions.h"
#include "../Math/Vec2Operators.h"
#include <map>

// Get the instance's current PCK File
PCKFile*& GetCurrentPCKFile();
ImGuiIO*& GetImGuiIO();

// Render and Maintain Menu Bar
void HandleMenuBar();

// Render and Maintain File Tree
void HandleFileTree();

// Reset PCK++ UI Data
void ResetUIData();

// Setup UI related Vendors
void UISetup();

// Cleanup UI related vendors
void UICleanup();