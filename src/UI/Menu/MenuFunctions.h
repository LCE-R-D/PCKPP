#pragma once

#include "../UI.h"

// function for opening a PCK File via path
void OpenPCKFile(const std::string& inpath);

// function for opening a PCK file via dialog
void OpenPCKFileDialog();

// function for saving a PCK File to disk
void SavePCKFile(const std::string& outpath, IO::Endianness endianness);

// function for saving a PCK File to a specified point on disk
void SavePCKFileDialog(IO::Endianness endianness, const std::string& defaultName);

// function to open a file dialog and read properties into a given file
void SetFilePropertiesDialog(PCKAssetFile& file);

// function to set data from file; returns success bool for processing
bool SetDataFromFile(PCKAssetFile& file);

// Save File Properties to File via path
void SaveFilePropertiesToFile(const PCKAssetFile& file, const std::string& outpath);

// Save File Properties to file via dialog
void SaveFilePropertiesDialog(const PCKAssetFile& file);

// Save File Data to file via Dialog
void ExtractFileDataDialog(const PCKAssetFile& file, bool includeProperties = false);