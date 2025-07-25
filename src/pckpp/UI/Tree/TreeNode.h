#pragma once
#include "../../PCK/PCKAssetFile.h"

struct FileTreeNode {
    std::string path{};
    PCKAssetFile* file{ nullptr };
    std::vector<FileTreeNode> children;
};