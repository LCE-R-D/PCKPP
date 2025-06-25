#include "BinaryReader.h"

BinaryReader::BinaryReader(const std::string& filepath)
	: mStream(filepath, std::ios::binary)
{
	if (!mStream) {
		throw std::runtime_error("Failed to open file: " + filepath);
	}
}

void BinaryReader::SetEndianness(IO::Endianness endianness)
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
	if (mEndianness == IO::Endianness::BIG)
		value = SwapInt16(value);
	return value;
}

uint32_t BinaryReader::ReadInt32()
{
	uint32_t value;
	ReadData(&value, sizeof(value));
	if (mEndianness == IO::Endianness::BIG)
		value = SwapInt32(value);
	return value;
}

std::string BinaryReader::ReadWideString(size_t length)
{
	std::u16string utf16str;
	utf16str.resize(length);

	for (size_t i = 0; i < length; ++i)
	{
		uint16_t ch;
		if (mEndianness == IO::Endianness::BIG)
		{
			uint8_t bytes[2];
			ReadData(bytes, 2);
			ch = (bytes[0] << 8) | bytes[1];
		}
		else
		{
			uint8_t bytes[2];
			ReadData(bytes, 2);
			ch = (bytes[1] << 8) | bytes[0];
		}
		utf16str[i] = ch;
	}

	// Convert UTF-16 to UTF-8
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
	return converter.to_bytes(utf16str);
}

void BinaryReader::ReadData(void* buffer, size_t size)
{
	mStream.read(reinterpret_cast<char*>(buffer), size);
	if (mStream.gcount() != size) {
		throw std::runtime_error("Failed to read from file.");
	}
}

uint16_t BinaryReader::SwapInt16(uint16_t val)
{
	return (val >> 8) | (val << 8);
}

uint32_t BinaryReader::SwapInt32(uint32_t val)
{
	return ((val & 0xFF) << 24) |
		((val & 0xFF00) << 8) |
		((val & 0xFF0000) >> 8) |
		((val & 0xFF000000) >> 24);
}