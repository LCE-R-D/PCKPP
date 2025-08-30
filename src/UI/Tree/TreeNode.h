#pragma once

#include <pckpp/PCK/PCKFile.h>

struct FileTreeNode {
    std::string path{};
    PCKAssetFile* file{ nullptr };
    std::vector<FileTreeNode> children;
};