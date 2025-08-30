#pragma once

#include <fstream>
#include <cstdint>

namespace Binary
{
	// Endianness... duh
	enum class Endianness
	{
		LITTLE,
		BIG,
		ENDIANNESS_TOTAL
	};

	// Encoding used for reading in Text files, like CSM and property .txt files
	enum class TextEncoding {
		UTF8,
		UTF16_LE,
		UTF16_BE,
		TEXT_ENCODING_TOTAL
	};

	// Detects Text encoding for 
	TextEncoding DetectTextEncoding(std::ifstream& in);

	// Swaps endianness of Int16 value
	uint16_t SwapInt16(const uint16_t value);

	// Swaps endianness of Int32 value
	uint32_t SwapInt32(const uint32_t value);

	// Swap endianness of UTF16 string
	void SwapUTF16Bytes(char16_t* buffer, size_t count);

	// Convert std::u16string to std::string
	std::string ToUTF8(const std::u16string& utf16);

	// Convert std::string to std::u16string 
	std::u16string ToUTF16(const std::string& utf8);
}