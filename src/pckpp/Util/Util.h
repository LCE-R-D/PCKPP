#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_dialog.h>
#include <algorithm>
#include <fstream>
#include <functional>
#include <string>
#include "../PCK/PCKAssetFile.h"
#include "../Binary/Binary.h"

namespace IO
{
	// Read file from disk to byte vector
	std::vector<unsigned char> ReadFile(const std::string& path);

	// Write file to disk from byte vector
	void WriteFile(const std::string& path, const std::vector<unsigned char>& fileData, const std::vector<PCKAssetFile::Property>& properties = {});
}

namespace String
{
	bool startsWith(const std::string& str, const std::string& prefix);
}