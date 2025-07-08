#pragma once

#include "../UI.h"

// function for opening a PCK File into the current instance; window is passed for child window popup stuff
void OpenPCKFile();

// function for saving a PCK File to disk
void SavePCKFile(const std::string& outpath, IO::Endianness endianness);

// function for saving a PCK File to a specified point on disk
void SavePCKFileAs(IO::Endianness endianness, const std::string& defaultName);