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
	~PCKFile() = default;

	void Read(const std::string& inpath);

	uint32_t getPCKVersion();

	IO::Endianness getEndianness();

	const std::vector<std::string>& getProperties() const;

	const std::vector<PCKAssetFile>& getFiles() const;

private:
	IO::Endianness mEndianess{ IO::Endianness::LITTLE };
	uint32_t mVersion{};
	std::vector<std::string> mProperties{};
	std::vector<PCKAssetFile> mFiles{};
};