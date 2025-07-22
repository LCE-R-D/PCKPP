#pragma once

#include <stdint.h>
#include <vector>
#include <filesystem>
#include "PCKAssetFile.h"
#include "../Binary/Binary.h"

// PCK File research done by Jam1Garner, Nobledez, NessieHax/Miku666/nullptr, myself (May/MattNL), and many others over the years.

class PCKFile
{
public:
	PCKFile() = default;
	~PCKFile();

	// Reads data into the PCK File from string; will add memory variant soon
	void Read(const std::string& inpath);

	// Writes PCK File to a specifed location
	void Write(const std::string& outpath, Binary::Endianness endianness);

	// Reads PCK Format/Version and sets Endianness
	uint32_t getPCKVersion() const;

	// Gets PCK File Endianness; Little Endian: Xbox One, PS4, PSVita, Nintendo Switch; Big Endian: Xbox 360, PS3, Wii U
	Binary::Endianness getEndianness() const;

	// Gets Registered Property Keys from the PCK File
	const std::vector<std::string>& getPropertyKeys() const;

	// Gets Files from the PCK File
	const std::vector<PCKAssetFile>& getFiles() const;

	// Adds PCKAssetFile to the PCK file
	void addFile(const PCKAssetFile* file);

	// Deletes PCKAssetFile from the PCK file
	void deleteFile(const PCKAssetFile* file);

	// Clears the PCK File
	void clearFiles();

	// Get XML Support. TODO: Replace with XMLVersion PROPERLY
	bool getXMLSupport() const;

	// Set XML Support. TODO: Replace with XMLVersion PROPERLY
	void setXMLSupport(bool value);

	// Get name of the file for display
	std::string getFileName() const;

	// Get the filepath
	std::string getFilePath() const;

	// Set the filepath
	void setFilePath(const std::string& pathin);

private:
	Binary::Endianness mEndianess{ Binary::Endianness::LITTLE };
	bool mXMLSupport{false};
	uint32_t mVersion{};
	std::vector<std::string> mProperties{};
	std::vector<PCKAssetFile> mFiles{};
	std::filesystem::path mFilePath{};
};