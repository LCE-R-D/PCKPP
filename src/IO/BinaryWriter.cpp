#include "BinaryWriter.h"

BinaryWriter::BinaryWriter(const std::string& filepath)
	: mStream(filepath, std::ios::binary)
{
	if (!mStream) {
		throw std::runtime_error("Failed to open file for writing: " + filepath);
	}
}

const void BinaryWriter::SetEndianness(IO::Endianness endianness)
{
	mEndianness = endianness;
}

void BinaryWriter::WriteInt8(uint8_t value)
{
	WriteData(&value, sizeof(value));
}

void BinaryWriter::WriteInt16(uint16_t value)
{
	if (mEndianness == IO::Endianness::BIG)
		value = SwapInt16(value);
	WriteData(&value, sizeof(value));
}

void BinaryWriter::WriteInt32(uint32_t value)
{
	if (mEndianness == IO::Endianness::BIG)
		value = SwapInt32(value);
	WriteData(&value, sizeof(value));
}

void BinaryWriter::WriteU16String(const std::u16string& utf16str)
{
	for (char16_t ch : utf16str)
	{
		uint8_t bytes[2]{};
		if (mEndianness == IO::Endianness::BIG)
		{
			bytes[0] = static_cast<uint8_t>((ch >> 8) & 0xFF);
			bytes[1] = static_cast<uint8_t>(ch & 0xFF);
		}
		else
		{
			bytes[0] = static_cast<uint8_t>(ch & 0xFF);
			bytes[1] = static_cast<uint8_t>((ch >> 8) & 0xFF);
		}
		WriteData(bytes, 2);
	}
}

const void BinaryWriter::WriteData(const void* buffer, size_t size)
{
	if (!mStream.write(reinterpret_cast<const char*>(buffer), size)) {
		throw std::runtime_error("Failed to write to file.");
	}
}

uint16_t BinaryWriter::SwapInt16(uint16_t val)
{
	return (val >> 8) | (val << 8);
}

uint32_t BinaryWriter::SwapInt32(uint32_t val)
{
	return ((val & 0xFF) << 24) |
		((val & 0xFF00) << 8) |
		((val & 0xFF0000) >> 8) |
		((val & 0xFF000000) >> 24);
}