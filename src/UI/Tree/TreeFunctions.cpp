#include "TreeFunctions.h"
#include <functional>

void TreeToPCKFileCollection(std::vector<FileTreeNode>& treeNodes)
{
	if (!GetCurrentPCKFile())
		return;

	std::vector<PCKAssetFile> files;

	std::function<void(const FileTreeNode&)> collect = [&](const FileTreeNode& node) {
		if (node.file)
			files.push_back(*node.file);

		for (const auto& child : node.children)
			collect(child);
		};

	// First collect root files
	for (const auto& node : treeNodes)
	{
		if (node.file)
			collect(node);
	}

	// Then collect from folders
	for (const auto& node : treeNodes)
	{
		if (!node.file)
			collect(node);
	}

	GetCurrentPCKFile()->clearFiles();

	for (const auto& f : files)
		GetCurrentPCKFile()->addFile(&f);

	files.clear();
}

FileTreeNode* FindNodeByPath(const std::string& path, std::vector<FileTreeNode>& nodes)
{
	for (auto& node : nodes)
	{
		if (node.path == path)
			return &node;

		if (FileTreeNode* found = FindNodeByPath(path, node.children))
			return found;
	}
	return nullptr;
}

void DeleteNode(FileTreeNode& targetNode, std::vector<FileTreeNode>& nodes)
{
	auto it = std::find_if(nodes.begin(), nodes.end(), [&](const FileTreeNode& n) {
		return &n == &targetNode;
		});

	if (it != nodes.end())
	{
		nodes.erase(it);
	}

	for (auto& node : nodes)
	{
		DeleteNode(targetNode, node.children);
	}
}

void SortTree(FileTreeNode& node) {
	std::stable_sort(node.children.begin(), node.children.end(), [](const FileTreeNode& a, const FileTreeNode& b) {
		if (!a.file && b.file) return true;
		if (a.file && !b.file) return false;
		if (!a.file && !b.file) return a.path < b.path;
		return false;
		});
	for (auto& child : node.children)
		if (!child.file)
			SortTree(child);
}

void BuildFileTree(std::vector<FileTreeNode>& nodes) {
	if (!GetCurrentPCKFile()) 
		return;

	nodes.clear();

	FileTreeNode root;
	auto& files = GetCurrentPCKFile()->getFiles();

	for (const auto& file : files) {
		std::string fullPath = file.getPath();
		size_t slashPos = fullPath.find_last_of("/\\");
		std::string folderName = (slashPos != std::string::npos) ? fullPath.substr(0, slashPos) : "";

		std::vector<std::string> parts;
		size_t start = 0;
		while (true) {
			size_t pos = folderName.find_first_of("/\\", start);
			if (pos == std::string::npos) {
				parts.push_back(folderName.substr(start));
				break;
			}
			parts.push_back(folderName.substr(start, pos - start));
			start = pos + 1;
		}

		FileTreeNode* current = &root;
		for (const auto& part : parts) {
			if (part.empty()) continue;
			auto it = std::find_if(current->children.begin(), current->children.end(), [&](const FileTreeNode& n) {
				return !n.file && n.path == part;
				});
			if (it == current->children.end()) {
				current->children.push_back(FileTreeNode{ part, nullptr });
				current = &current->children.back();
			}
			else {
				current = &(*it);
			}
		}
		current->children.push_back(FileTreeNode{ file.getPath(), const_cast<PCKAssetFile*>(&file) });
	}

	SortTree(root);
	nodes = std::move(root.children);
}

void ScrollToNode(bool& keyScrolled)
{
	if (!keyScrolled)
		return;

	float itemMin = ImGui::GetItemRectMin().y;
	float itemMax = ImGui::GetItemRectMax().y;
	float viewMin = ImGui::GetWindowPos().y;
	float viewMax = viewMin + ImGui::GetWindowSize().y - ImGui::GetItemRectSize().y;

	// Scroll only if item is outside the visible region
	if (itemMin < viewMin || itemMax > viewMax)
	{
		ImGui::SetScrollHereY(0.5f); // Center it
	}

	keyScrolled = false;
}

void SavePCK(std::vector<FileTreeNode> nodes, IO::Endianness endianness, const std::string& path, const std::string& defaultName)
{
	TreeToPCKFileCollection(nodes);

	if (!path.empty()) {
		SavePCKFile(path, endianness);
	}
	else {
		SavePCKFileDialog(endianness, defaultName);
	}
}

void SaveFolderAsFiles(const FileTreeNode& node, bool includeProperties)
{
	std::string targetDir = IO::ChooseFolderDialog(GetWindow(), "Choose Output Directory");
	if (targetDir.empty())
	{
		ShowCancelledMessage();
		return;
	}

	try {
		std::function<void(const FileTreeNode&, const std::string&)> saveRecursive =
			[&](const FileTreeNode& n, const std::string& currentPath)
			{
				if (!n.file)
				{
					std::string folderPath = currentPath + "/" + n.path;
					std::filesystem::create_directories(folderPath);

					for (const auto& child : n.children)
						saveRecursive(child, folderPath);
				}
				else
				{
					std::string fileName = std::filesystem::path(n.path).filename().string();
					std::string filePath = currentPath + "/" + fileName;

					std::ofstream outFile(filePath, std::ios::binary);
					if (outFile)
						outFile.write(reinterpret_cast<const char*>(n.file->getData().data()), n.file->getFileSize());

					if (outFile.good() && includeProperties)
					{
						SaveFilePropertiesToFile(*n.file, filePath + ".txt");
					}
				}
			};

		saveRecursive(node, targetDir);
	}
	catch (...)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", SDL_GetError(), GetWindow());
	}
}