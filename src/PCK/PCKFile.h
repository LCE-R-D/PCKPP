#pragma once

#include <stdint.h>
#include <vector>
#include "PCKAssetFile.h"
#include "../IO/IO.h"

// PCK File research done by Jam1Garner, Nobledez, NessieHax/Miku666/nullptr, myself (May/MattNL), and many others over the years.

class PCKFile
{
public:
	PCKFile() = default;
	~PCKFile();

	// Reads data into the PCK File from string; will add memory variant soon
	void Read(const std::string& inpath);

	// Writes PCK File to a specifed location
	void Write(const std::string& outpath, IO::Endianness endianness);

	// Reads PCK Format/Version and sets Endianness
	uint32_t getPCKVersion() const;

	// Gets PCK File Endianness; Little Endian: Xbox One, PS4, PSVita, Nintendo Switch; Big Endian: Xbox 360, PS3, Wii U
	IO::Endianness getEndianness() const;

	// Gets Registered Property Keys from the PCK File
	const std::vector<std::string>& getPropertyKeys() const;

	// Gets Files from the PCK File
	const std::vector<PCKAssetFile>& getFiles() const;

	void addFile(const PCKAssetFile* file);

	void deleteFile(const PCKAssetFile* file);

	void clearFiles();

	bool getXMLSupport() const;

	void setXMLSupport(bool value);

private:
	IO::Endianness mEndianess{ IO::Endianness::LITTLE };
	bool mXMLSupport{false};
	uint32_t mVersion{};
	std::vector<std::string> mProperties{};
	std::vector<PCKAssetFile> mFiles{};
};