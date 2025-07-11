#pragma once

#include "../UI.h"

// function for opening a PCK File into the current instance; window is passed for child window popup stuff
void OpenPCKFile(const std::string& inpath);

void OpenPCKFileDialog();

// function for saving a PCK File to disk
void SavePCKFile(const std::string& outpath, IO::Endianness endianness);

// function for saving a PCK File to a specified point on disk
void SavePCKFileAs(IO::Endianness endianness, const std::string& defaultName);

// function to open a file dialog and read properties into a given file
void SetPropertiesFromFile(PCKAssetFile& file);

// function to set data from file; returns success bool for processing
bool SetDataFromFile(PCKAssetFile& file);