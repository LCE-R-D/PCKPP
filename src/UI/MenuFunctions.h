#pragma once

#include "Binary/Binary.h"
#include "PCK/PCKAssetFile.h"
#include "Platform/PlatformBase.h"

// Opens PCK File from path
void OpenPCKFile(const std::string& inpath);

// Opens PCK File via file dialog
void OpenPCKFileDialog();

// Saves PCK File via file dialog
void SavePCKFileDialog(Binary::Endianness endianness, const std::string& defaultName);

// Saves PCK File to path
void SavePCKFile(const std::string& outpath, Binary::Endianness endianness);

// Replaces file properties via file dialog
void SetFilePropertiesDialog(PCKAssetFile& file);

// Gets file dialog filters from a file's path
PlatformBase::FileDialogBase::FileFilter GetFilter(const PCKAssetFile& file);

// Sets file data via file dialog
bool SetFileDataDialog(PCKAssetFile& file);

// Writes file properties to path
void WriteFileProperties(const PCKAssetFile& file, const std::string& outpath);

// Writes file properties via file dialog
void WriteFilePropertiesDialog(const PCKAssetFile& file);

// Writes file data to disk via file dialog
void WriteFileDataDialog(const PCKAssetFile& file, bool includeProperties = false);