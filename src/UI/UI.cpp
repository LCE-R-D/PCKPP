#include "UI.h"
#include <iomanip>
#include <sstream>
#include <map>

struct FileTreeNode {
	std::string path{};
	PCKAssetFile* file{ nullptr }; // folder by default
	std::vector<FileTreeNode> children;
};

// Resource globals
std::map<PCKAssetFile::Type, Texture> gFileIcons;
std::vector<FileTreeNode> gTreeNodes;
std::vector<const FileTreeNode*> gVisibleNodes;
Texture gFolderIcon;

// Preview globals
Texture gPreviewTexture{};
std::string gPreviewTitle = "Preview";

// Instance globals
PCKFile* gCurrentPCK{ nullptr };
std::string gSelectedPath;
bool gKeyboardScrolled{ false };
ImGuiIO* io{ nullptr };
float gMainMenuBarHeight{ 24.0f };
static bool gShouldOpenFolder = false;
static bool gShouldCloseFolder = false;

PCKFile*& GetCurrentPCKFile() { return gCurrentPCK; }
ImGuiIO*& GetImGuiIO() { return io; }

// Handles the menu bar, functions are held in MenuFunctions.h/cpp
void HandleMenuBar() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open", "Ctrl+O")) {
				OpenPCKFile(GetWindow());
			}
			if (gCurrentPCK && ImGui::MenuItem("Save", "Ctrl+S")) {
				// Save code here
			}
			ImGui::EndMenu();
		}
		gMainMenuBarHeight = ImGui::GetFrameHeight();
		ImGui::EndMainMenuBar();
	}
}

// Grabs the filename directly from path... I should probably be using filesystem lol
static std::string GetFileNameFromPath(const std::string& fullPath) {
	size_t pos = fullPath.find_last_of("/\\");
	return (pos == std::string::npos) ? fullPath : fullPath.substr(pos + 1);
}

// Sorts the file tree; First by folders, then by files; folders are sorted alphabetically, and files are not sorted at all
static void SortTree(FileTreeNode& node) {
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

// Builds the file tree
static void BuildFileTree() {
	if (!gCurrentPCK) return;
	gTreeNodes.clear();

	FileTreeNode root;
	auto& files = gCurrentPCK->getFiles();

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
	gTreeNodes = std::move(root.children);
}

// Renders and handles window to preview the currently selected file if any data is previewable
static void HandlePreviewWindow(const PCKAssetFile& file) {
	static const PCKAssetFile* lastPreviewedFile = nullptr;
	static bool zoomChanged = false;
	static float userZoom = 1.0f;

	// if ID is valid AND last file is not the current file
	if (gPreviewTexture.id != 0 && lastPreviewedFile != &file) {
		glDeleteTextures(1, &gPreviewTexture.id);
		gPreviewTexture = {};
		zoomChanged = false;
		userZoom = 1.0f;
	}

	if (lastPreviewedFile != &file) {
		gPreviewTexture = LoadTextureFromMemory(file.getData().data(), file.getFileSize());
		lastPreviewedFile = &file;
		gPreviewTitle = file.getPath() + " (" + std::to_string(gPreviewTexture.width) + "x" + std::to_string(gPreviewTexture.height) + ")###Preview";

		userZoom = 1.0f;
		zoomChanged = false;
	}

	if (gPreviewTexture.id == 0) return;

	float previewPosX = io->DisplaySize.x * 0.25f;
	ImVec2 windowSize(io->DisplaySize.x * 0.75f, io->DisplaySize.y - (io->DisplaySize.y * 0.25f));
	ImGui::SetNextWindowPos(ImVec2(previewPosX, gMainMenuBarHeight), ImGuiCond_Always);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

	ImGui::Begin(gPreviewTitle.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::BeginChild("PreviewScroll", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

	if (ImGui::IsWindowHovered() && io->KeyCtrl && io->MouseWheel != 0.0f) {
		float zoomDelta = io->MouseWheel * 0.1f;
		userZoom = std::clamp(userZoom * (1.0f + zoomDelta), 0.5f, 100.0f); // this clamp is a little weird but it works lol
		zoomChanged = true;
	}

	ImVec2 availSize = ImGui::GetContentRegionAvail();

	if (!zoomChanged && userZoom == 1.0f) {
		userZoom = std::min(
			(availSize.x) / gPreviewTexture.width,
			(availSize.y) / gPreviewTexture.height
		);
	}

	ImVec2 imageSize = ImVec2(gPreviewTexture.width * userZoom, gPreviewTexture.height * userZoom);

	ImVec2 cursorPos = ImGui::GetCursorPos();
	if (imageSize.x < availSize.x) cursorPos.x += (availSize.x - imageSize.x) / 2.0f;
	if (imageSize.y < availSize.y) cursorPos.y += (availSize.y - imageSize.y) / 2.0f;
	ImGui::SetCursorPos(cursorPos);
	ImGui::Image((ImTextureID)(intptr_t)gPreviewTexture.id, imageSize);

	std::stringstream ss;
	ss << "Zoom: " << std::fixed << std::setprecision(1) << (userZoom) << "%";
	std::string zoomText = ss.str();
	ImVec2 textSize = ImGui::CalcTextSize(zoomText.c_str());
	ImVec2 textPos = ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x - textSize.x - 20, ImGui::GetWindowPos().y);
	ImGui::SetCursorScreenPos(textPos);
	ImGui::TextUnformatted(zoomText.c_str());

	ImGui::EndChild();
	ImGui::End();
}

// Scrolls to selected node when not visible
static void ScrollToNode()
{
	if (!gKeyboardScrolled)
		return;

	float itemMin = ImGui::GetItemRectMin().y;
	float itemMax = ImGui::GetItemRectMax().y;
	float viewMin = ImGui::GetWindowPos().y;
	float viewMax = viewMin + ImGui::GetWindowSize().y;

	// Scroll if node is not fully visible
	if (itemMin < viewMax || itemMax > viewMin)
	{
		ImGui::SetScrollHereY(0.5f);
	}

	gKeyboardScrolled = false;
}

static void SaveNodeAsFile(const FileTreeNode& node, bool includeProperties = false)
{
	const std::string& path = node.file->getPath();

	std::string ext = path.substr(path.find_last_of('.') + 1);

	// Fallback extension if none found
	if (ext.empty() || ext == path)
		ext = "";

	// Use static so memory stays valid while the dialog is open
	static std::string nameStr;
	static std::string patternStr;

	nameStr = std::string(node.file->getAssetTypeString()) + " | *." + ext + " File";
	patternStr = ext;

	SDL_DialogFileFilter filter{};
	filter.name = nameStr.c_str();
	filter.pattern = patternStr.c_str();

	IO::SaveFileDialog(GetWindow(), &filter, node.file->getData(), GetFileNameFromPath(node.file->getPath()), true, node.file->getProperties());
}

static void HandlePCKNodeContextMenu(const FileTreeNode& node)
{
	if (node.file && ImGui::BeginPopupContextItem()) {
		if (ImGui::BeginMenu("Extract")) {
			if (ImGui::MenuItem("File"))
			{
				SaveNodeAsFile(node);
			}

			bool hasProperties = node.file->getProperties().size() > 0;

			if (hasProperties && ImGui::MenuItem("Properties"))
			{
				const std::string& path = node.file->getPath();

				std::string ext = path.substr(path.find_last_of('.') + 1);

				// Use static so memory stays valid while the dialog is open
				static std::string nameStr;
				static std::string patternStr;

				nameStr = "Text File | *." + ext + ".txt" + " File";
				patternStr = ext + ".txt";

				SDL_DialogFileFilter filter{};
				filter.name = nameStr.c_str();
				filter.pattern = patternStr.c_str();

				std::string propertyData{};

				for (auto& p : node.file->getProperties())
				{
					propertyData += p.first + ' ' + p.second + '\n';
				}

				IO::SaveFileDialog(GetWindow(), &filter, { propertyData.begin(), propertyData.end() }, GetFileNameFromPath(node.file->getPath()) + ".txt", true);
			}
			if (hasProperties && ImGui::MenuItem("File with Properties"))
			{
				SaveNodeAsFile(node, true);
			}
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}
}

// Renders the passed node, also handles children or the node
static void RenderNode(const FileTreeNode& node, std::vector<const FileTreeNode*>* visibleList = nullptr) {
	if (visibleList)
		visibleList->push_back(&node); // adds node to visible nodes vector, but only when needed

	bool isFolder = (node.file == nullptr);
	bool isSelected = (node.path == gSelectedPath);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
	if (isSelected)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
		ScrollToNode();
	}

	if (isFolder) {
		ImGui::PushID(node.path.c_str());
		ImGui::Image((void*)(intptr_t)gFolderIcon.id, ImVec2(48, 48));
		ImGui::SameLine();
		if (node.path == gSelectedPath) {
			if (gShouldOpenFolder) ImGui::SetNextItemOpen(true, ImGuiCond_Always);
			else if (gShouldCloseFolder) ImGui::SetNextItemOpen(false, ImGuiCond_Always);
		}
		bool open = ImGui::TreeNodeEx(node.path.c_str(), flags);
		if (ImGui::IsItemClicked()) gSelectedPath = node.path;
		if (open) {
			for (const auto& child : node.children)
				RenderNode(child, visibleList);
			ImGui::TreePop();
		}
		ImGui::PopID();
	}
	else {
		const PCKAssetFile& file = *node.file;
		ImGui::Image((void*)(intptr_t)gFileIcons[file.getAssetType()].id, ImVec2(48, 48));
		ImGui::SameLine();
		if (ImGui::Selectable((GetFileNameFromPath(file.getPath()) + "###" + file.getPath()).c_str(), isSelected))
			gSelectedPath = node.path;
	}

	HandlePCKNodeContextMenu(node);
}

// Finds the currently selected file
static const PCKAssetFile* FindSelectedFile(const FileTreeNode& node) {
	if (node.path == gSelectedPath && node.file)
		return node.file;
	for (const auto& child : node.children) {
		const PCKAssetFile* result = FindSelectedFile(child);
		if (result) return result;
	}
	return nullptr;
}

// Renders the file tree... duh
static void RenderFileTree() {
	if (!gCurrentPCK) return;

	gVisibleNodes.clear();
	ImGui::SetNextWindowPos(ImVec2(0, gMainMenuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(io->DisplaySize.x * 0.25f, io->DisplaySize.y - gMainMenuBarHeight));
	ImGui::Begin("File Tree", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	gShouldOpenFolder = false;
	gShouldCloseFolder = false;

	if (ImGui::IsWindowFocused() && !gSelectedPath.empty()) {
		if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) gShouldOpenFolder = true;
		else if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) gShouldCloseFolder = true;
	}

	for (const auto& node : gTreeNodes) RenderNode(node, &gVisibleNodes);

	static int selectedIndex = -1;
	if (ImGui::IsWindowFocused() && !gVisibleNodes.empty()) {
		for (int i = 0; i < (int)gVisibleNodes.size(); ++i) {
			if (gVisibleNodes[i]->path == gSelectedPath) {
				selectedIndex = i;
				break;
			}
		}

		std::string previousPath = gSelectedPath;
		if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
			selectedIndex = std::max(0, selectedIndex - 1);
			gSelectedPath = gVisibleNodes[selectedIndex]->path;
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
			selectedIndex = std::min((int)gVisibleNodes.size() - 1, selectedIndex + 1);
			gSelectedPath = gVisibleNodes[selectedIndex]->path;
		}
		if (gSelectedPath != previousPath)
			gKeyboardScrolled = true;
	}

	const PCKAssetFile* selectedFile = nullptr;
	for (const auto& node : gTreeNodes) {
		selectedFile = FindSelectedFile(node);
		if (selectedFile) break;
	}
	if (selectedFile && selectedFile->isImageType()) HandlePreviewWindow(*selectedFile);

	gShouldOpenFolder = false;
	gShouldCloseFolder = false;
	ImGui::End();
}

void HandleFileTree() {
	BuildFileTree();
	RenderFileTree();
}

void UISetup() {
	gFolderIcon = LoadTextureFromFile("assets/icons/NODE_FOLDER.png", GL_LINEAR_MIPMAP_LINEAR);
	for (int i = 0; i < (int)PCKAssetFile::Type::PCK_ASSET_TYPES_TOTAL; i++) {
		auto type = static_cast<PCKAssetFile::Type>(i);
		std::string name = (type == PCKAssetFile::Type::UI_DATA)
			? PCKAssetFile::getAssetTypeString(PCKAssetFile::Type::PCK_ASSET_TYPES_TOTAL)
			: PCKAssetFile::getAssetTypeString(type);
		std::string path = "assets/icons/FILE_" + name + ".png";
		gFileIcons[type] = LoadTextureFromFile(path, GL_LINEAR_MIPMAP_LINEAR);
	}
	io = &ImGui::GetIO();
	io->Fonts->AddFontFromFileTTF("assets/fonts/m6x11plus.ttf", 18.0f);
}

void ResetUIData() {

	gSelectedPath = "";
	gKeyboardScrolled = false;
	gShouldOpenFolder = false;
	gShouldCloseFolder = false;

	gTreeNodes.clear();
	gVisibleNodes.clear();
}

void UICleanup() {
	ResetUIData();
	for (auto& [type, tex] : gFileIcons)
		if (tex.id != 0)
			glDeleteTextures(1, &tex.id);
	gFileIcons.clear();
}
