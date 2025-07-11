#include "MenuFunctions.h"
#include "../UI.h"

static SDL_DialogFileFilter pckFilter[] = {
	{ "Minecraft LCE DLC Files (*.pck)", "pck" }
};

void OpenPCKFile(const std::string& inpath)
{
	if (inpath.empty())
		return;

	PCKFile*& currentPCKFile = GetCurrentPCKFile();

	delete currentPCKFile;
	currentPCKFile = new PCKFile();

	try {
		currentPCKFile->Read(inpath);
	}
	catch (const std::exception& e) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", e.what(), GetWindow());
		delete currentPCKFile;
		currentPCKFile = new PCKFile();
		return;
	}
	catch (...) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Unknown Error Occured.", GetWindow());
		delete currentPCKFile;
		currentPCKFile = new PCKFile();
		return;
	}

	// if successful, reset node and UI data; pass file path to send to UI
	ResetUIData(inpath);
}

void OpenPCKFileDialog()
{
	std::string filePath = IO::OpenFileDialog(GetWindow(), pckFilter);

	if (!filePath.empty())
		OpenPCKFile(filePath);
	else
		ShowCancelledMessage();
}

void SavePCKFileDialog(IO::Endianness endianness, const std::string& defaultName)
{
	PCKFile*& currentPCKFile = GetCurrentPCKFile();

	std::string filePath = IO::SaveFileDialog(GetWindow(), pckFilter, defaultName);

	if(!filePath.empty())
		SavePCKFile(filePath, endianness);
	else
		ShowCancelledMessage();
}

void SavePCKFile(const std::string& outpath, IO::Endianness endianness)
{
	PCKFile*& currentPCKFile = GetCurrentPCKFile();

	try {
		currentPCKFile->Write(outpath, endianness);
	}
	catch (const std::exception& e) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", e.what(), GetWindow());
		delete currentPCKFile;
		currentPCKFile = new PCKFile();
	}

	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Saved", "File successfully saved!", GetWindow());
}

void SetFilePropertiesDialog(PCKAssetFile& file)
{
	const static SDL_DialogFileFilter filters[] = {
	{ "Text File", "txt" },
	{ "All Files", "*" }
	};

	std::string inpath = IO::OpenFileDialog(GetWindow(), filters);

	if (!inpath.empty())
	{
		std::ifstream in(inpath, std::ios::binary);
		if (in)
		{
			file.clearProperties();
			IO::TextEncoding encoding = IO::DetectTextEncoding(in);

			std::string key;
			std::u16string value;

			if (encoding == IO::TextEncoding::UTF8)
			{
				// Skip BOM if present
				char first3[3] = { 0 };
				in.read(first3, 3);
				if (!((unsigned char)first3[0] == 0xEF && (unsigned char)first3[1] == 0xBB && (unsigned char)first3[2] == 0xBF))
					in.seekg(0);

				std::string line;
				while (std::getline(in, line))
				{
					if (line.empty()) continue;

					std::istringstream iss(line);
					if (!(iss >> key)) continue;

					// Strip trailing colon, if any
					if (!key.empty() && key.back() == u':')
						key.pop_back();

					std::string value8;
					std::getline(iss, value8);
					value8.erase(0, value8.find_first_not_of(" \t"));

					value = IO::ToUTF16(value8);

					file.addProperty(key, value);
				}
			}
			else if (encoding == IO::TextEncoding::UTF16_LE || encoding == IO::TextEncoding::UTF16_BE)
			{
				// Skip BOM (2 bytes)
				in.seekg(2);

				in.seekg(0, std::ios::end);
				size_t fileSize = in.tellg();
				in.seekg(2, std::ios::beg);

				size_t numChars = (fileSize - 2) / 2;
				std::vector<char16_t> buffer(numChars);

				in.read(reinterpret_cast<char*>(buffer.data()), numChars * 2);

				if (encoding == IO::TextEncoding::UTF16_BE)
					IO::SwapUTF16Bytes(buffer.data(), numChars);

				size_t start = 0;
				for (size_t i = 0; i <= numChars; ++i)
				{
					if (i == numChars || buffer[i] == u'\n')
					{
						std::u16string line(buffer.data() + start, i - start);
						start = i + 1;

						if (line.empty()) continue;
						if (!line.empty() && line.back() == u'\r')
							line.pop_back();

						size_t key_end = 0;
						while (key_end < line.size() && line[key_end] != u' ' && line[key_end] != u'\t')
							++key_end;

						std::u16string key16 = line.substr(0, key_end);

						// Strip trailing colon, if any
						if (!key16.empty() && key16.back() == u':')
							key16.pop_back();
						if (key_end < line.size())
						{
							value = line.substr(key_end);

							// trimming the whitespace
							size_t pos = 0;
							while (pos < value.size() && (value[pos] == u' ' || value[pos] == u'\t')) ++pos;
							if (pos > 0) value.erase(0, pos);
						}

						key = IO::ToUTF8(key16);
					}
				}
			}
			if (key.empty()) // key cannot be empty; value can be empty, and is expected sometimes, like in the case of Texture ANIMs
			{
				file.addProperty(key, value);
			}
		}
	}
	else
		ShowCancelledMessage();
}

bool SetDataFromFile(PCKAssetFile& file)
{
	std::filesystem::path filePath(file.getPath());

	std::string ext = filePath.extension().string();
	if (!ext.empty() && ext[0] == '.')
		ext.erase(0, 1);

	std::string label = std::string(file.getAssetTypeString()) + " File | *." + ext + " File";
	SDL_DialogFileFilter filters[] = {
		{ label.c_str(), ext.c_str() },
		{ "All Files", "*" }
	};

	std::string inpath = IO::OpenFileDialog(GetWindow(), filters);

	if (!inpath.empty())
	{
		std::ifstream in(inpath, std::ios::binary | std::ios::ate);
		if (in)
		{
			std::streamsize size = in.tellg();
			in.seekg(0, std::ios::beg);
			std::vector<unsigned char> buffer(size);

			in.read(reinterpret_cast<char*>(buffer.data()), size);
			if (in.gcount() == size)
			{
				file.setData(buffer);
			}
			in.close();

			return true;
		}
	}

	ShowCancelledMessage();

	return false;
}

void SaveFilePropertiesToFile(const PCKAssetFile& file, const std::string& outpath)
{
	if (outpath.empty())
	{
		return;
	}

	std::u16string propertyData;
	for (const auto& [key, val] : file.getProperties()) {
		propertyData += IO::ToUTF16(key) + u' ' + val + u'\n';
	}

	std::ofstream propFile(outpath, std::ios::binary);
	if (propFile.is_open())
	{
		const char* buffer = reinterpret_cast<const char*>(propertyData.data());
		std::size_t byteCount = propertyData.size() * sizeof(char16_t);
		propFile.write(buffer, byteCount);
		propFile.close();
	}
}

void SaveFilePropertiesDialog(const PCKAssetFile& file)
{
	static const std::string nameStr = "Text File | *.txt";
	static const std::string patternStr = "txt";

	SDL_DialogFileFilter filter{};
	filter.name = nameStr.c_str();
	filter.pattern = patternStr.c_str();

	std::string outpath = IO::SaveFileDialog(GetWindow(), &filter, std::filesystem::path(file.getPath()).filename().string() + ".txt");

	if (!outpath.empty())
	{
		SaveFilePropertiesToFile(file, outpath);
		ShowSuccessMessage();
	}
	else
		ShowCancelledMessage();
}

void ExtractFileDataDialog(const PCKAssetFile& file, bool includeProperties)
{
	std::filesystem::path filePath(file.getPath());

	std::string ext = filePath.extension().string();
	if (!ext.empty() && ext[0] == '.')
		ext.erase(0, 1);

	static std::string nameStr;
	static std::string patternStr;

	nameStr = std::string(file.getAssetTypeString()) + " | *." + ext + " File";
	patternStr = ext;

	SDL_DialogFileFilter filter{};
	filter.name = nameStr.c_str();
	filter.pattern = patternStr.c_str();

	std::string outPath{};

	// this is very dumb and I'll have to give this a rewrite sometime
	if (includeProperties)
		outPath = IO::SaveFileDialogWithProperties(GetWindow(), &filter, file.getData(), filePath.filename().string(), true, file.getProperties());
	else
		outPath = IO::SaveFileDialogWithProperties(GetWindow(), &filter, file.getData(), filePath.filename().string());

	if (!outPath.empty())
		ShowSuccessMessage();
	else
		ShowCancelledMessage();
}