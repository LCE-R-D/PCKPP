#pragma once

#include <fstream>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include <locale>
#include <codecvt>
#include "IO.h"

// Barebones binary reader because it's nice I guess; inspired by miku666/NessieHax/nullptr's EndiannessAwareBinaryReader from the OMI/PCK Studio code <3
class BinaryReader
{
public:
	BinaryReader(const std::string& filepath);
	~BinaryReader()
	{
		if (mStream)
			mStream.close();
	}

	void SetEndianness(IO::Endianness endianness);

	uint8_t ReadInt8();
	uint16_t ReadInt16();
	uint32_t ReadInt32();

	std::string ReadWideString(size_t length);

	static uint16_t SwapInt16(uint16_t value);
	static uint32_t SwapInt32(uint32_t value);

	void ReadData(void* buffer, size_t size);

private:
	std::ifstream mStream;
	IO::Endianness mEndianness = IO::Endianness::LITTLE; // default to little since Little is used by more editions of the game
};