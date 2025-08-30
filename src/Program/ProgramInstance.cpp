#include "Application/Application.h"
#include "Program/ProgramInstance.h"

void ProgramInstance::Reset() {
    if (mCurrentPCKFile) {
        hasXMLSupport = mCurrentPCKFile->getXMLSupport();
        pckEndianness = mCurrentPCKFile->getEndianness();
    }

    selectedNodePath.clear();
}

PCKFile* ProgramInstance::GetCurrentPCKFile() {
    return mCurrentPCKFile.get();
}

void ProgramInstance::LoadPCKFile(const std::string& filepath) {
    bool empty = !mCurrentPCKFile;

    try {
        mCurrentPCKFile = std::make_unique<PCKFile>();
        mCurrentPCKFile->Read(filepath);
    }
    catch (...) {
        printf("Failed to load PCK file: %s", filepath.c_str());
        if(empty) mCurrentPCKFile = nullptr; // so when an file argument fails, the UI isn't already loaded
    }
}