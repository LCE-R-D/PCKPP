#pragma once

#include <fstream>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include <locale>
#include <codecvt>
#include "IO.h"

// Barebones binary writer because it's nice I guess; inspired by miku666/NessieHax/nullptr's EndiannessAwareBinaryWriter from the OMI/PCK Studio code <3
class BinaryWriter
{
public:
	BinaryWriter(const std::string& filepath);
	~BinaryWriter()
	{
		if (mStream)
			mStream.close();
	}

	const void SetEndianness(IO::Endianness endianness);

	void WriteInt8(const uint8_t value);
	void WriteInt16(const uint16_t value);
	void WriteInt32(const uint32_t value);

	void WriteWideString(const std::u16string& utf16str);

	static uint16_t SwapInt16(const uint16_t value);
	static uint32_t SwapInt32(const uint32_t value);

	const void WriteData(const void* buffer, size_t size);

private:
	std::ofstream mStream;
	IO::Endianness mEndianness = IO::Endianness::LITTLE; // default to little since Little is used by more editions of the game
};