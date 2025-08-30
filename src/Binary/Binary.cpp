#include <locale>
#include <codecvt>
#include "Binary/Binary.h"

uint16_t Binary::SwapInt16(uint16_t value)
{
	return (value >> 8) | (value << 8);
}

uint32_t Binary::SwapInt32(uint32_t value)
{
	return ((value & 0xFF) << 24) |
		((value & 0xFF00) << 8) |
		((value & 0xFF0000) >> 8) |
		((value & 0xFF000000) >> 24);
}

void Binary::SwapUTF16Bytes(char16_t* buffer, size_t count) {
	for (size_t i = 0; i < count; ++i) {
		char16_t& c = buffer[i];
		c = (c >> 8) | (c << 8);
	}
}

Binary::TextEncoding Binary::DetectTextEncoding(std::ifstream& in) {
	char bom[4] = { 0 };
	in.read(bom, 4);
	size_t readBytes = in.gcount();
	in.seekg(0);

	if (readBytes >= 3 && (unsigned char)bom[0] == 0xEF && (unsigned char)bom[1] == 0xBB && (unsigned char)bom[2] == 0xBF)
		return Binary::TextEncoding::UTF8;

	if (readBytes >= 2 && (unsigned char)bom[0] == 0xFF && (unsigned char)bom[1] == 0xFE)
		return Binary::TextEncoding::UTF16_LE;

	if (readBytes >= 2 && (unsigned char)bom[0] == 0xFE && (unsigned char)bom[1] == 0xFF)
		return Binary::TextEncoding::UTF16_BE;

	// no BOM found, it's UTF-8 by default
	return Binary::TextEncoding::UTF8;
}

std::string Binary::ToUTF8(const std::u16string& utf16) {
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return convert.to_bytes(utf16);
}

std::u16string Binary::ToUTF16(const std::string& utf8) {
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return convert.from_bytes(utf8);
}