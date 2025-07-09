#include "PCKFile.h"
#include "../IO/BinaryReader.h"
#include "../IO/BinaryWriter.h"
#include <set>

const char* XML_VERSION_STRING{ "XMLVERSION" }; // used for advanced/full box support for skins

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

	for (uint32_t i{ 0 }; i < propertyCount; i++)
	{
		uint32_t propertyIndex = reader.ReadInt32();
		uint32_t stringLength = reader.ReadInt32();

		std::string property = IO::ToUTF8(reader.ReadU16String(stringLength));

		SDL_Log("\tIndex: %u, Property: %s", propertyIndex, property.c_str());

		mProperties.push_back(property);

		reader.ReadInt32(); // skip 4 bytes
	}

	mXMLSupport = std::any_of(mProperties.begin(), mProperties.end(),
		[](const std::string& property) { return property == XML_VERSION_STRING; });

	if (mXMLSupport) {
		reader.ReadInt32(); // just "skip" 4 bytes
		SDL_Log("XML Version: %u", mXMLSupport);
	}

	uint32_t fileCount = reader.ReadInt32();

	// temporary file size vector to hold sizes for now
	std::vector<uint32_t> fileSizes{};

	for (uint32_t i{ 0 }; i < fileCount; i++)
	{
		uint32_t fileSize = reader.ReadInt32();
		uint32_t fileType = reader.ReadInt32();
		uint32_t filePathLength = reader.ReadInt32();

		std::string filePath = IO::ToUTF8(reader.ReadU16String(filePathLength));
		std::replace(filePath.begin(), filePath.end(), '\\', '/');

		reader.ReadInt32(); // skip 4 bytes

		mFiles.emplace_back(filePath, PCKAssetFile::Type(fileType));
		fileSizes.push_back(fileSize);
	}

	SDL_Log("Files: %u", fileCount);

	for (int i{ 0 }; i < mFiles.size(); ++i)
	{
		PCKAssetFile& file = mFiles[i];
		uint32_t propertyCount = reader.ReadInt32();

		SDL_Log("\tSize: %u Bytes | Type: %u | Properties: %u | Path: %s", fileSizes[i], (uint32_t)file.getAssetType(), propertyCount, file.getPath().c_str());

		for (int j{ 0 }; j < propertyCount; j++)
		{
			uint32_t propertyIndex = reader.ReadInt32();

			if (propertyIndex >= mProperties.size()) {
				throw std::runtime_error("Property index out of range");
			}

			std::string propertyKey = mProperties[propertyIndex];
			uint32_t propertyValueLength = reader.ReadInt32();
			std::u16string propertyValue = reader.ReadU16String(propertyValueLength);

			reader.ReadInt32(); // skip 4 bytes

			SDL_Log("\t\tProperty: %s %s", propertyKey.c_str(), IO::ToUTF8(propertyValue).c_str());

			file.addProperty(propertyKey, propertyValue);
		}

		std::vector<unsigned char> fileData(fileSizes[i]);
		reader.ReadData(fileData.data(), fileData.size());
		file.setData(std::move(fileData));
	}
}

void PCKFile::Write(const std::string& outpath, IO::Endianness endianness)
{
	BinaryWriter writer(outpath);
	writer.SetEndianness(endianness);

	uint32_t versionOut = mVersion;
	if (endianness != IO::Endianness::LITTLE)
		versionOut = BinaryWriter::SwapInt32(mVersion);
	writer.WriteData(&versionOut, sizeof(uint32_t));

	// make new property list
	mProperties.clear();
	std::set<std::string> propertySet;

	if (mXMLSupport)
		mProperties.push_back(XML_VERSION_STRING);

	for (const auto& file : mFiles)
	{
		for (const auto& [key, _] : file.getProperties())
		{
			if (propertySet.insert(key).second) // only insert if not already in set
				mProperties.push_back(key);
		}
	}

	uint32_t propertyCount = static_cast<uint32_t>(mProperties.size());
	writer.WriteInt32(propertyCount);

	for (uint32_t i = 0; i < propertyCount; ++i)
	{
		writer.WriteInt32(i);
		writer.WriteInt32(static_cast<uint32_t>(mProperties[i].size()));
		writer.WriteU16String(IO::ToUTF16(mProperties[i]));
		writer.WriteInt32(0); // skip 4 bytes
	}

	if (mXMLSupport)
	{
		writer.WriteInt32(3); // this is just for now until other XMLVersions are supported
	}

	uint32_t fileCount = static_cast<uint32_t>(mFiles.size());
	writer.WriteInt32(fileCount);

	for (const auto& file : mFiles)
	{
		writer.WriteInt32(static_cast<uint32_t>(file.getFileSize()));
		writer.WriteInt32(static_cast<uint32_t>(file.getAssetType()));

		const std::string& filePath = file.getPath();
		writer.WriteInt32(static_cast<uint32_t>(filePath.size()));
		writer.WriteU16String(IO::ToUTF16(filePath));
		writer.WriteInt32(0); // skip 4 bytes
	}

	for (const auto& file : mFiles)
	{
		const auto& props = file.getProperties();
		writer.WriteInt32(static_cast<uint32_t>(props.size()));

		for (const auto& [key, value] : props)
		{
			// key must be in global mProperties
			auto it = std::find(mProperties.begin(), mProperties.end(), key);
			uint32_t index = static_cast<uint32_t>(std::distance(mProperties.begin(), it));
			writer.WriteInt32(index);
			writer.WriteInt32(static_cast<uint32_t>(value.size()));
			writer.WriteU16String(value);
			writer.WriteInt32(0); // skip 4 bytes
		}

		writer.WriteData(file.getData().data(), file.getFileSize());
	}
}

uint32_t PCKFile::getPCKVersion() const
{
	return mVersion;
}

IO::Endianness PCKFile::getEndianness() const
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

bool PCKFile::getXMLSupport() const
{
	return mXMLSupport;
}

void PCKFile::setXMLSupport(bool value)
{
	mXMLSupport = value;
}

PCKFile::~PCKFile()
{
	mProperties.clear();
	mFiles.clear();
}