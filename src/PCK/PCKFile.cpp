#include "PCKFile.h"
#include "../IO/BinaryReader.h"

const char* XML_VERSION_STRING{"XMLVERSION"}; // used for advanced/full box support for skins

void PCKFile::Read(const std::string& inpath)
{
	BinaryReader reader(inpath);

	uint32_t version;
	reader.ReadData(&version, 4); // assume this is little endian

	uint32_t versionSwapped = BinaryReader::SwapInt32(version); // swapped for endianness check; assume big endian

	if (versionSwapped >= 0 && versionSwapped <= 3)
	{
		mEndianess = IO::Endianness::BIG;
		SDL_Log("Big Endian detected, version %u", versionSwapped);
		mVersion = versionSwapped;
	}
	else if (version >= 0 && version <= 3)
	{
		mEndianess = IO::Endianness::LITTLE;
		SDL_Log("Little Endian detected, version %u", version);
		mVersion = version;
	}
	else
	{
		throw std::runtime_error("Invalid PCK version");
	}

	reader.SetEndianness(mEndianess);

	uint32_t propertyCount = reader.ReadInt32();
	SDL_Log("Properties: %u", propertyCount);

	mProperties.clear();
	mProperties.reserve(propertyCount);

	for (uint32_t i{0}; i < propertyCount; i++)
	{
		uint32_t propertyIndex = reader.ReadInt32();
		uint32_t stringLength = reader.ReadInt32();

		std::string property = reader.ReadWideString(stringLength);

		SDL_Log("\tIndex: %u, Property: %s", propertyIndex, property.c_str());

		mProperties.push_back(property);

		reader.ReadInt32(); // skip 4 bytes
	}

	bool found = std::any_of(mProperties.begin(), mProperties.end(),
		[](const std::string& property) { return property == XML_VERSION_STRING; });

	if (found) {
		mXMLVersion = reader.ReadInt32();
		SDL_Log("XML Version: %u", mXMLVersion);
	}

	uint32_t fileCount = reader.ReadInt32();

	// temporary file size vector to hold sizes for now
	std::vector<uint32_t> fileSizes{};

	for (uint32_t i{ 0 }; i < fileCount; i++)
	{
		uint32_t fileSize = reader.ReadInt32();
		uint32_t fileType = reader.ReadInt32();
		uint32_t filePathLength = reader.ReadInt32();

		std::string filePath = reader.ReadWideString(filePathLength);
		std::replace(filePath.begin(), filePath.end(), '\\', '/');

		reader.ReadInt32(); // skip 4 bytes

		mFiles.emplace_back(filePath, PCKAssetFile::Type(fileType));
		fileSizes.push_back(fileSize);
	}

	SDL_Log("Files: %u", fileCount);

	for (int i{0}; i < mFiles.size(); ++i)
	{
		PCKAssetFile& file = mFiles[i];
		uint32_t propertyCount = reader.ReadInt32();

		SDL_Log("\tSize: %u Bytes | Type: %u | Properties: %u | Path: %s", fileSizes[i], (uint32_t)file.getAssetType(), propertyCount, file.getPath().c_str());

		for (int j{ 0 }; j < propertyCount; j++)
		{
			uint32_t propertyIndex = reader.ReadInt32();
			std::string propertyKey = mProperties[propertyIndex];
			uint32_t propertyValueLength = reader.ReadInt32();
			std::string propertyValue = reader.ReadWideString(propertyValueLength);

			reader.ReadInt32(); // skip 4 bytes

			SDL_Log("\t\tProperty: %s %s", propertyKey.c_str(), propertyValue.c_str());

			file.addProperty(propertyKey, propertyValue);
		}

		std::vector<unsigned char> fileData(fileSizes[i]);
		reader.ReadData(fileData.data(), fileData.size());
		file.setData(std::move(fileData));
	}
}

uint32_t PCKFile::getPCKVersion()
{
	return mVersion;
}

IO::Endianness PCKFile::getEndianness()
{
	return mEndianess;
}

const std::vector<std::string>& PCKFile::getPropertyKeys() const
{
	return mProperties;
}

const std::vector<PCKAssetFile>& PCKFile::getFiles() const
{
	return mFiles;
}

PCKFile::~PCKFile()
{
	mProperties.clear();
	mFiles.clear();
}