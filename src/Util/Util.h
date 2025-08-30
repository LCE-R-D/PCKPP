#pragma once

#include "PCK/PCKAssetFile.h"
#include <codecvt>
#include <locale>
#include <sstream>

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

	std::wstring toWstring(const std::u16string& u16);
}