#include "../Application/Application.h"
#include "ProgramInstance.h"

void ProgramInstance::Reset() {
    PCKFile* pckFile = gApp->GetInstance()->GetCurrentPCKFile();

    if (pckFile) {
        hasXMLSupport = pckFile->getXMLSupport();
        pckEndianness = pckFile->getEndianness();
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