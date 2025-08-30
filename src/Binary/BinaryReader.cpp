#include "Binary/BinaryReader.h"

BinaryReader::BinaryReader(const std::string& filepath)
	: mStream(filepath, std::ios::binary)
{
	if (!mStream) {
		throw std::runtime_error("Failed to open file: " + filepath);
	}
}

void BinaryReader::SetEndianness(Binary::Endianness endianness)
{
	mEndianness = endianness;
}

uint8_t BinaryReader::ReadInt8()
{
	uint8_t value;
	ReadData(&value, sizeof(value));
	return value;
}

uint16_t BinaryReader::ReadInt16()
{
	uint16_t value;
	ReadData(&value, sizeof(value));
	if (mEndianness == Binary::Endianness::BIG)
		value = Binary::SwapInt16(value);
	return value;
}

uint32_t BinaryReader::ReadInt32()
{
	uint32_t value;
	ReadData(&value, sizeof(value));
	if (mEndianness == Binary::Endianness::BIG)
		value = Binary::SwapInt32(value);
	return value;
}

std::u16string BinaryReader::ReadU16String(size_t length)
{
	std::u16string utf16str;
	utf16str.resize(length);

	for (size_t i = 0; i < length; ++i)
	{
		uint16_t ch;
		uint8_t bytes[2];
		ReadData(bytes, 2);

		if (mEndianness == Binary::Endianness::BIG)
			ch = (bytes[0] << 8) | bytes[1];
		else
			ch = (bytes[1] << 8) | bytes[0];

		utf16str[i] = ch;
	}

	return utf16str;
}

void BinaryReader::ReadData(void* buffer, size_t size)
{
	mStream.read(reinterpret_cast<char*>(buffer), size);
	if (mStream.gcount() != size) {
		throw std::runtime_error("Failed to read from file.");
	}
}