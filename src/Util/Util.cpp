#include "Binary/Binary.h"
#include "Util/Util.h"

std::vector<unsigned char> IO::ReadFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return {};
    return std::vector<unsigned char>(std::istreambuf_iterator<char>(file), {});
}

void IO::WriteFile(const std::string& path, const std::vector<unsigned char>& fileData, const std::vector<PCKAssetFile::Property>& properties) {
    std::ofstream ofile(path, std::ios::binary);
    if (!ofile.is_open()) {
        printf("Failed to open file for writing: %s\n", path.c_str());
        return;
    }

    ofile.write(reinterpret_cast<const char*>(fileData.data()), fileData.size());
    ofile.close();

    if (!properties.empty()) {
        std::ofstream propFile(path + ".txt", std::ios::binary);
        if (propFile.is_open()) {
            std::u16string propertyData;
            for (const auto& [key, val] : properties)
                propertyData += Binary::ToUTF16(key) + u' ' + val + u'\n';

            std::vector<unsigned char> bytes(propertyData.begin(), propertyData.end());
            propFile.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
            propFile.close();
        }
    }
}

bool String::startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

std::wstring String::toWstring(const std::u16string& u16) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
    return std::wstring(conv.to_bytes(u16).begin(), conv.to_bytes(u16).end());
}