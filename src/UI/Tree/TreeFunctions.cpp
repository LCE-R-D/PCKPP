#include <functional>
#include "UI/MenuFunctions.h"
#include "UI/Tree/TreeFunctions.h"
#include "Util/Util.h"

void TreeToPCKFileCollection(std::vector<FileTreeNode>& treeNodes)
{
	PCKFile* pckFile = gApp->GetInstance()->GetCurrentPCKFile();

	if (!pckFile)
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

	pckFile->clearFiles();

	for (const auto& f : files)
		pckFile->addFile(&f);

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

void RenameDirectory(const std::string& targetPath, const std::string& newName, std::vector<FileTreeNode>& nodes)
{
	for (auto& node : nodes)
	{
		if (node.file)
		{
			std::string oldPath = node.file->getPath();
			std::replace(oldPath.begin(), oldPath.end(), '\\', '/');

			if (String::startsWith(oldPath, (targetPath)))
			{
				std::string subPath = oldPath.substr(targetPath.size());

				// Remove double slashies
				if (!subPath.empty() && subPath[0] == '/')
					subPath.erase(0, 1);

				std::filesystem::path baseNewName(newName);
				std::filesystem::path sub(subPath);
				std::filesystem::path newPath = baseNewName / sub;

				std::string newPathStr = newPath.generic_string();

				printf("Renaming: %s -> %s\n", oldPath.c_str(), newPathStr.c_str());

				node.file->setPath(newPathStr);
			}
		}

		RenameDirectory(targetPath, newName, node.children);
	}
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

void BuildFileTree() {
	PCKFile* pckFile = gApp->GetInstance()->GetCurrentPCKFile();

	if (!pckFile)
		return;

	gApp->GetInstance()->treeNodes.clear();

	FileTreeNode root;
	auto& files = pckFile->getFiles();

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
		std::filesystem::path currentPath = root.path;

		for (const auto& part : parts) {
			if (part.empty()) continue;

			currentPath /= part;

			std::string normalizedCurrent = currentPath.string();
			std::replace(normalizedCurrent.begin(), normalizedCurrent.end(), '\\', '/');

			auto it = std::find_if(current->children.begin(), current->children.end(), [&](const FileTreeNode& n) {
				return !n.file && n.path == normalizedCurrent;
				});

			if (it == current->children.end()) {
				current->children.push_back(FileTreeNode{ normalizedCurrent, nullptr });
				current = &current->children.back();
			}
			else {
				current = &(*it);
			}
		}

		std::filesystem::path filePath = file.getPath();
		current->children.push_back(FileTreeNode{ filePath.string(), const_cast<PCKAssetFile*>(&file) });
	}

	SortTree(root);
	gApp->GetInstance()->treeNodes = std::move(root.children);
}

// TODO: Move to UIBase
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

void SavePCK(std::vector<FileTreeNode> nodes, Binary::Endianness endianness, const std::string& path, const std::string& defaultName)
{
	TreeToPCKFileCollection(nodes);

	if (!path.empty()) {
		SavePCKFile(path, endianness);
	}
	else {
		SavePCKFileDialog(endianness, defaultName);
	}
}

void WriteFolder(const FileTreeNode& node, bool includeProperties)
{
	const auto& platform = gApp->GetPlatform();

	std::string targetDir = platform->mDialog.ChooseFolder();
	if (targetDir.empty())
	{
		platform->ShowCancelledMessage();
		return;
	}

	try {
		std::function<void(const FileTreeNode&, const std::string&)> saveRecursive =
			[&](const FileTreeNode& n, const std::string& currentPath)
			{
				if (!n.file)
				{
					std::string folderPath = currentPath + "/" + std::filesystem::path(n.path).filename().string();
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
						WriteFileProperties(*n.file, filePath + ".txt");
					}
				}
			};

		saveRecursive(node, targetDir);
	}
	catch (...)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", SDL_GetError(), platform->GetWindow());
	}
}