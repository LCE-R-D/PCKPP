#include "../Application/Application.h"
#include "ProgramInstance.h"

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
    try {
        mCurrentPCKFile = std::make_unique<PCKFile>();
        mCurrentPCKFile->Read(filepath);
    }
    catch (...) {
        printf("Failed to load PCK file: %s", filepath.c_str());
    }
}