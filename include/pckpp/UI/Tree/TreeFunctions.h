#pragma once

#include <pckpp/Application/Application.h>
#include <pckpp/Binary/Binary.h>
#include <pckpp/PCK/PCKAssetFile.h>
#include <pckpp/UI/Tree/TreeNode.h>

// Saves PCK file from tree nodes
void SavePCK(std::vector<FileTreeNode> nodes, Binary::Endianness endianness, const std::string& path = "", const std::string& defaultName = "");

// Writes folder of nodes to disk via file dialog
void WriteFolder(const FileTreeNode& node, bool includeProperties = false);

// Convert file tree to PCK File collection
void TreeToPCKFileCollection(std::vector<FileTreeNode>& treeNodes);

// Finds a node by path in a given file tree
FileTreeNode* FindNodeByPath(const std::string& path, std::vector<FileTreeNode>& nodes);

// Deletes a node in a given file tree
void DeleteNode(FileTreeNode& targetNode, std::vector<FileTreeNode>& nodes);

// Sorts a given file tree
void SortTree(FileTreeNode& node);

// Builds a given file tree
void BuildFileTree();

// Scrolls to selected node when not visible
void ScrollToNode(bool& keyScrolled);