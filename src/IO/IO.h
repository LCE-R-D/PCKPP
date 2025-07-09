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

	// For converting std::u16string to std::string (UTF-8)
	std::string ToUTF8(const std::u16string& str);

	// For converting std::string to std::u16string
	std::u16string ToUTF16(const std::string& str);

	// For getting the path for opening files with SDL's file system
	std::string OpenFileDialog(SDL_Window* window, SDL_DialogFileFilter* filters);

	// For getting the path for saving files with SDL's file system
	std::string SaveFileDialog(SDL_Window* window, SDL_DialogFileFilter* filters, const std::string& defaultName);

	// For saving files with SDL's file system AND writing with support for Properties
	std::string SaveFileDialogWithProperties(SDL_Window* window, SDL_DialogFileFilter* filters, const std::vector<unsigned char>& fileData, const std::string& defaultName, bool ignoreExt = false, const std::vector<std::pair<std::string, std::u16string>>& properties = {});
}