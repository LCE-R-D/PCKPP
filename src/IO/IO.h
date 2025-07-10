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

	enum class TextEncoding { // Used for reading in Text files, like CSM and properties files
		UTF8,
		UTF16_LE,
		UTF16_BE,
		TEXT_ENCODING_TOTAL
	};

	// Swap bytes for UTF-16 Encoded BE to LE conversion
	void SwapUTF16Bytes(char16_t* buffer, size_t count);

	// Detects encoding using by looking for a text file's Byte Order Maker (BOM)
	TextEncoding DetectTextEncoding(std::ifstream& in);

	// For converting std::u16string to std::string (UTF-8)
	std::string ToUTF8(const std::u16string& str);

	// For converting std::string to std::u16string
	std::u16string ToUTF16(const std::string& str);

	// For getting the path for opening files with SDL's file system
	std::string OpenFileDialog(SDL_Window* window, SDL_DialogFileFilter* filters);

	// For getting the path for saving files with SDL's file system
	std::string SaveFileDialog(SDL_Window* window, SDL_DialogFileFilter* filters, const std::string& defaultName);

	// For saving files with SDL's file system AND writing with support for Properties
	std::string SaveFileDialogWithProperties(SDL_Window* window, SDL_DialogFileFilter* filters, const std::vector<unsigned char>& fileData, const std::string& defaultName, bool ignoreExt = false, const std::vector<PCKAssetFile::Property>& properties = {});

	// For choosing a folder to work with
	std::string ChooseFolderDialog(SDL_Window* window, const std::string& title);
}