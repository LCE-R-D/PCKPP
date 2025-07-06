#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_dialog.h>
#include <algorithm>
#include <fstream>
#include <functional>
#include <string>

namespace IO
{
	enum class Endianness
	{
		LITTLE,
		BIG,
		ENDIANESS_TOTAL
	};

	// For opening files with SDL's file system
	std::string OpenFileDialog(SDL_Window* window, SDL_DialogFileFilter* filters);

	// For saving files with SDL's file system
	void SaveFileDialog(SDL_Window* window, SDL_DialogFileFilter* filters, const std::vector<unsigned char>& fileData, const std::string& defaultName, bool ignoreExt = false, const std::vector<std::pair<std::string, std::string>>& properties = {});
}