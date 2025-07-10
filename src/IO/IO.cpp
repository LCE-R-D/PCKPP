#include "IO.h"
#include <SDL3/SDL_log.h>
#include <condition_variable>
#include <vector>
#include <codecvt>
#include <filesystem>
#include "../PCK/PCKAssetFile.h"

static std::mutex gMutex;
static std::condition_variable gConditionVariable;
static std::string gSelectedFile;
static std::atomic<bool> gDialogFinished(false);

std::string IO::ToUTF8(const std::u16string& str) {
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return convert.to_bytes(str);
}

std::u16string IO::ToUTF16(const std::string& str) {
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return convert.from_bytes(str);
}

static void ChooseFolderDialogCallback(void*, const char* const* filelist, int)
{
	std::lock_guard<std::mutex> lock(gMutex);

	if (filelist && *filelist)
		gSelectedFile = *filelist;
	else
		gSelectedFile.clear();

	gDialogFinished = true;
	gConditionVariable.notify_one();
}

std::string IO::ChooseFolderDialog(SDL_Window* window, const std::string& title)
{
	gDialogFinished = false;
	gSelectedFile.clear();

	SDL_ShowSaveFileDialog(ChooseFolderDialogCallback, nullptr, window, nullptr, 0, title.c_str());

	SDL_Event event;
	while (!gDialogFinished)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT) {
				gDialogFinished = true;
			}
		}

		std::unique_lock<std::mutex> lock(gMutex);
		gConditionVariable.wait_for(lock, std::chrono::milliseconds(10), [] { return gDialogFinished.load(); });
	}

	// Strip filename, return directory path only
	if (!gSelectedFile.empty()) {
		std::filesystem::path path(gSelectedFile);
		return path.parent_path().string();
	}

	return {};
}

// Because SDL needs this
static void SaveFileDialogCallback(void* userdata, const char* const* filelist, int)
{
	std::lock_guard<std::mutex> lock(gMutex);

	if (!filelist || !*filelist) {
		gDialogFinished = true;
		SDL_Log("Save dialog cancelled or invalid file parameters passed.");
		return;
	}

	gSelectedFile = *filelist;

	// Check userdata
	if (!userdata) {
		gDialogFinished = true;
		gConditionVariable.notify_one();
		return;
	}

	auto* data = static_cast<std::tuple<const char*, std::vector<unsigned char>, bool, std::vector<PCKAssetFile::Property>>*>(userdata);

	const char* extension = std::get<0>(*data);
	std::vector<unsigned char>& fileData = std::get<1>(*data);
	bool ignoreExtension = std::get<2>(*data);
	std::vector<PCKAssetFile::Property>& properties = std::get<3>(*data);

	std::filesystem::path filepath = *filelist;

	// Automatically append extension if missing
	if (!ignoreExtension && filepath.extension() != extension) {
		filepath.replace_extension(extension);
	}

	SDL_Log("Save file path: %s", filepath.string().c_str());

	std::ofstream ofile(filepath, std::ios::binary);
	if (ofile.is_open()) {
		ofile.write(reinterpret_cast<const char*>(fileData.data()), fileData.size());
		ofile.close();

		// Write associated properties if present
		if (!properties.empty()) {
			std::ofstream propertiesFile(filepath.string() + ".txt", std::ios::binary);
			if (propertiesFile.is_open()) {
				std::u16string propertyData;
				for (const auto& [key, val] : properties) {
					// ensure all this is u16
					propertyData += IO::ToUTF16(key) + u' ' + val + u'\n';
				}
				std::vector<unsigned char> bytes{ propertyData.begin(), propertyData.end() };
				propertiesFile.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
				propertiesFile.close();
			}
			else {
				SDL_Log("Failed to open properties file for writing: %s.txt", filepath.string().c_str());
			}
		}
	}
	else {
		SDL_Log("Failed to open file for writing: %s", filepath.string().c_str());
	}

	delete data;

	gDialogFinished = true;
	gConditionVariable.notify_one();
}

// Because SDL needs this
static void OpenFileDialogCallback(void*, const char* const* filelist, int)
{
	std::lock_guard<std::mutex> lock(gMutex);
	if (filelist && *filelist) {
		gSelectedFile = *filelist;
	}
	else {
		gSelectedFile.clear();
	}
	gDialogFinished = true;
	gConditionVariable.notify_one();
}

std::string IO::OpenFileDialog(SDL_Window* window, SDL_DialogFileFilter* filters)
{
	gDialogFinished = false;
	gSelectedFile.clear();

	SDL_ShowOpenFileDialog(OpenFileDialogCallback, nullptr, window, filters, sizeof(*filters)/sizeof(filters[0]), nullptr, false);

	SDL_Event event;
	while (!gDialogFinished)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT) {
				gDialogFinished = true;
			}
		}

		std::unique_lock<std::mutex> lock(gMutex);
		gConditionVariable.wait_for(lock, std::chrono::milliseconds(10), [] { return gDialogFinished.load(); });
	}

	return gSelectedFile;
}

// Gets output path
std::string IO::SaveFileDialog(SDL_Window* window, SDL_DialogFileFilter* filters, const std::string& defaultName)
{
	gDialogFinished = false;
	gSelectedFile.clear();

	SDL_ShowSaveFileDialog(SaveFileDialogCallback, nullptr, window, filters, 1, defaultName.c_str());

	SDL_Event event;
	while (!gDialogFinished)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT) {
				gDialogFinished = true;
			}
		}

		std::unique_lock<std::mutex> lock(gMutex);
		gConditionVariable.wait_for(lock, std::chrono::milliseconds(10), [] { return gDialogFinished.load(); });
	}

	return gSelectedFile;
}

// Gets output path AND writes to file/disk
std::string IO::SaveFileDialogWithProperties(SDL_Window * window, SDL_DialogFileFilter * filters, const std::vector<unsigned char>& fileData, const std::string& defaultName, bool ignoreExt, const std::vector<PCKAssetFile::Property>& properties)
{
	gDialogFinished = false;
	gSelectedFile.clear();

	// this is a kinda silly way to do this
	auto* data = new std::tuple<const char*, std::vector<unsigned char>, bool, std::vector<PCKAssetFile::Property>>(filters->pattern, fileData, ignoreExt, properties);

	SDL_ShowSaveFileDialog(SaveFileDialogCallback, data, window, filters, 1, defaultName.c_str());

	SDL_Event event;
	while (!gDialogFinished)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT) {
				gDialogFinished = true;
			}
		}

		std::unique_lock<std::mutex> lock(gMutex);
		gConditionVariable.wait_for(lock, std::chrono::milliseconds(10), [] { return gDialogFinished.load(); });
	}

	return gSelectedFile;
}

void IO::SwapUTF16Bytes(char16_t* buffer, size_t count) {
	for (size_t i = 0; i < count; ++i) {
		char16_t& c = buffer[i];
		c = (c >> 8) | (c << 8);
	}
}

// Detects encoding using by looking for a text file's Byte Order Maker (BOM)
IO::TextEncoding IO::DetectTextEncoding(std::ifstream& in) {
	char bom[4] = { 0 };
	in.read(bom, 4);
	size_t readBytes = in.gcount();
	in.seekg(0);

	if (readBytes >= 3 && (unsigned char)bom[0] == 0xEF && (unsigned char)bom[1] == 0xBB && (unsigned char)bom[2] == 0xBF)
		return TextEncoding::UTF8;

	if (readBytes >= 2 && (unsigned char)bom[0] == 0xFF && (unsigned char)bom[1] == 0xFE)
		return TextEncoding::UTF16_LE;

	if (readBytes >= 2 && (unsigned char)bom[0] == 0xFE && (unsigned char)bom[1] == 0xFF)
		return TextEncoding::UTF16_BE;

	// no BOM found, it's UTF-8 by default
	return TextEncoding::UTF8;
}