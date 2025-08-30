#pragma once

#include "Binary/Binary.h"
#include "PCK/PCKFile.h"
#include "UI/Tree/TreeNode.h"

class ProgramInstance {
public:
    // The current selected path in the program, if any at all
    std::string selectedNodePath;

    // Is the XML Support checkbox clicked? TODO: Please May just make this XMLVersion already what are you waiting for you absolute bimbo, stop writing these stupid worthless comments that no one is reading and just do your damn job, what's even the point of these internal dialogues???
    bool hasXMLSupport = false;

    // Resets values in the instance to default
    Binary::Endianness pckEndianness = Binary::Endianness::LITTLE;

    // Resets values in the instance to default
    void Reset();

    // Get current PCK file
    PCKFile* GetCurrentPCKFile();

    // Loads PCK File from path
    void LoadPCKFile(const std::string& filepath);

    // Tree Nodes
    std::vector<FileTreeNode> treeNodes;

    // Tree Nodes that are visible (for keyboard navigation)
    std::vector<const FileTreeNode*> visibleNodes;

private:
    std::unique_ptr<PCKFile> mCurrentPCKFile;
};