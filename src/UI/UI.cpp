#include "UI.h"

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
static const PCKAssetFile* gLastPreviewedFile = nullptr;

// Instance globals
PCKFile* gCurrentPCK{ nullptr };
static std::string gCurrentPCKFilePath;
static std::string gCurrentPCKFileName;
static std::string gSelectedPath;
static bool gKeyboardScrolled{ false };
ImGuiIO* io{ nullptr };
float gMainMenuBarHeight{ 24.0f };
static bool gShouldOpenFolder{ false };
static bool gShouldCloseFolder{false};
static bool gHasXMLSupport{ false };
static IO::Endianness gPCKEndianness{ IO::Endianness::LITTLE };
static int gSelectedPropertyIndex = -1;

PCKFile*& GetCurrentPCKFile() { return gCurrentPCK; }
ImGuiIO*& GetImGuiIO() { return io; }

void TreeToPCKFiles()
{
	if (!gCurrentPCK)
		return;

	std::vector<PCKAssetFile> files;

	std::function<void(const FileTreeNode&)> collect = [&](const FileTreeNode& node) {
		if (node.file)
			files.push_back(*node.file);

		for (const auto& child : node.children)
			collect(child);
		};

	// First collect root files
	for (const auto& node : gTreeNodes)
	{
		if (node.file)
			collect(node);
	}

	// Then collect from folders
	for (const auto& node : gTreeNodes)
	{
		if (!node.file)
			collect(node);
	}

	gCurrentPCK->clearFiles();

	for (const auto& f : files)
		gCurrentPCK->addFile(&f);

	files.clear();
}

static void SavePCK(IO::Endianness endianness, const std::string& path = "", const std::string& defaultName = "")
{
	TreeToPCKFiles();

	if (!path.empty()) {
		SavePCKFile(path, endianness);
	}
	else {
		SavePCKFileAs(endianness, defaultName);
	}
}

FileTreeNode* FindNodeByPath(const std::string& path, std::vector<FileTreeNode>& nodes = gTreeNodes)
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

void DeleteNode(FileTreeNode& targetNode, std::vector<FileTreeNode>& nodes = gTreeNodes)
{
	auto it = std::find_if(nodes.begin(), nodes.end(), [&](const FileTreeNode& n) {
		return &n == &targetNode;
		});

	if (it != nodes.end())
	{
		nodes.erase(it);
		TreeToPCKFiles(); // rebuild tree after deletion
	}

	for (auto& node : nodes)
	{
		DeleteNode(targetNode, node.children);
	}
}

void HandleInput()
{
	// make sure to pass false or else it will trigger multiple times
	if (gCurrentPCK && ImGui::IsKeyPressed(ImGuiKey_Delete, false)) {
		if (ShowYesNoMessagePrompt("Are you sure?", "This is permanent and cannot be undone.\nIf this is a folder, all sub-files will be deleted too.")) {
			if (FileTreeNode* node = FindNodeByPath(gSelectedPath))
				DeleteNode(*node);
		}
	}

	if (io->KeyCtrl)
	{
		if (ImGui::IsKeyPressed(ImGuiKey_O, false)) {
			OpenPCKFileDialog();
		}
		else if (gCurrentPCK && io->KeyShift && ImGui::IsKeyPressed(ImGuiKey_S, false)) {
			SavePCK(gPCKEndianness, gCurrentPCKFilePath); // Save
		}
		else if (gCurrentPCK && ImGui::IsKeyPressed(ImGuiKey_S, false)) {
			SavePCK(gPCKEndianness, "", gCurrentPCKFileName); // Save As
		}
	}
}

void ResetPreviewWindow()
{
	glDeleteTextures(1, &gPreviewTexture.id);
	gPreviewTexture = {};
	gLastPreviewedFile = nullptr;
}

// Handles the menu bar, functions are held in MenuFunctions.h/cpp
void HandleMenuBar() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open", "Ctrl+O")) {
				OpenPCKFileDialog();
			}
			if (ImGui::MenuItem("Save", "Ctrl+S", nullptr, gCurrentPCK)) {
				SavePCK(gPCKEndianness, gCurrentPCKFilePath);
			}
			if (ImGui::MenuItem("Save as", "Ctrl+Shift+S", nullptr, gCurrentPCK)) {
				SavePCK(gPCKEndianness, "", gCurrentPCKFileName);
			}
			ImGui::EndMenu();
		}

		if (gCurrentPCK)
		{
			if (ImGui::BeginMenu("PCK"))
			{
				ImGui::Text("PCK Format:");
				if (ImGui::RadioButton("Little Endian (Xbox One, PS4, PSVita, Nintendo Switch)", gPCKEndianness == IO::Endianness::LITTLE))
				{
					gPCKEndianness = IO::Endianness::LITTLE;
				}

				if (ImGui::RadioButton("Big Endian (Xbox 360, PS3, Wii U)", gPCKEndianness == IO::Endianness::BIG))
				{
					gPCKEndianness = IO::Endianness::BIG;
				}

				ImGui::NewLine();
				if (ImGui::Checkbox("Full BOX Support (for Skins)", &gHasXMLSupport)) {
					gCurrentPCK->setXMLSupport(gHasXMLSupport);
				}

				ImGui::EndMenu();
			}
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

static void HandlePropertiesContextWindow(PCKAssetFile& file, int propertyIndex = -1)
{
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Add"))
		{
			file.addProperty("KEY", u"VALUE");
		}
		if (propertyIndex > -1 && ImGui::MenuItem("Delete"))
		{
			file.removeProperty(propertyIndex);
		}
		ImGui::EndPopup();
	}
}

static void HandlePropertiesWindow(const PCKAssetFile& file)
{
	if (gLastPreviewedFile != &file) {
		gLastPreviewedFile = &file;
	}

	const auto properties = file.getProperties(); // make a copy of properties

	float propertyWindowPosX = io->DisplaySize.x * 0.25f;
	float propertyWindowHeight = (io->DisplaySize.y * 0.35f) - gMainMenuBarHeight;
	ImVec2 propertyWindowSize(io->DisplaySize.x * 0.75f, propertyWindowHeight);
	ImGui::SetNextWindowSize(propertyWindowSize, ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(propertyWindowPosX, io->DisplaySize.y - propertyWindowHeight), ImGuiCond_Always);

	ImGui::Begin("Properties", nullptr,
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_HorizontalScrollbar);

	// very stupid lol
	PCKAssetFile& editableFile = const_cast<PCKAssetFile&>(file);

	if (ImGui::BeginPopupContextWindow("PropertiesContextWindow", ImGuiPopupFlags_MouseButtonRight))
	{
		if (ImGui::MenuItem("Add"))
			editableFile.addProperty("KEY", u"VALUE");
		ImGui::EndPopup();
	}

	if (properties.empty()) {
		ImGui::Text("NO PROPERTIES");
	}
	else {
		int propertyIndex = 0;

		if (ImGui::BeginTable("PropertiesTable", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV))
		{
			float maxKeyWidth = ImGui::CalcTextSize("KEY").x;
			for (const auto& [key, _] : properties)
			{
				ImVec2 size = ImGui::CalcTextSize(key.c_str());
				if (size.x > maxKeyWidth)
					maxKeyWidth = size.x;
			}
			maxKeyWidth += 10.0f;

			ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, maxKeyWidth);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			int propertyIndex = 0;
			for (const auto& [key, value] : properties)
			{
				ImGui::TableNextRow();

				char keyBuffer[0x11];
				char valueBuffer[0x1001];

				std::strncpy(keyBuffer, key.c_str(), sizeof(keyBuffer) - 1);
				keyBuffer[sizeof(keyBuffer) - 1] = '\0';

				std::string utf8Value = IO::ToUTF8(value);
				std::size_t len = std::min(utf8Value.size(), sizeof(valueBuffer) - 1);
				std::memcpy(valueBuffer, utf8Value.data(), len);
				valueBuffer[len] = '\0';

				bool modified = false;

				ImGui::TableSetColumnIndex(0);
				std::string keyLabel = "##Key" + std::to_string(propertyIndex);
				ImGui::SetNextItemWidth(-FLT_MIN); // this is needed to make the input the full size of the column for some reason
				if (ImGui::InputText(keyLabel.c_str(), keyBuffer, sizeof(keyBuffer)))
					modified = true;

				// context menu
				HandlePropertiesContextWindow(editableFile, propertyIndex);

				ImGui::TableSetColumnIndex(1);
				std::string valueLabel = "##Value" + std::to_string(propertyIndex);
				ImGui::SetNextItemWidth(-FLT_MIN);
				if (ImGui::InputText(valueLabel.c_str(), valueBuffer, sizeof(valueBuffer)))
					modified = true;

				// context menu; again because I want it to work with both rows
				HandlePropertiesContextWindow(editableFile, propertyIndex);

				if (modified)
				{
					std::string keyText = keyBuffer;
					for (char& c : keyText)
						c = std::toupper(c);

					editableFile.setPropertyAtIndex(propertyIndex, keyText.empty() ? "KEY" : keyText, IO::ToUTF16(valueBuffer));
				}

				++propertyIndex;
			}

			ImGui::EndTable();
		}
	}

	ImGui::End();
}

// Renders and handles window to preview the currently selected file if any data is previewable
static void HandlePreviewWindow(const PCKAssetFile& file) {
	static bool zoomChanged = false;
	static float userZoom = 1.0f;

	// if ID is valid AND last file is not the current file
	if (gPreviewTexture.id != 0 && gLastPreviewedFile != &file) {
		ResetPreviewWindow();
		zoomChanged = false;
		userZoom = 1.0f;
	}

	if (gLastPreviewedFile != &file) {
		gPreviewTexture = LoadTextureFromMemory(file.getData().data(), file.getFileSize());
		gLastPreviewedFile = &file;
		gPreviewTitle = file.getPath() + " (" + std::to_string(gPreviewTexture.width) + "x" + std::to_string(gPreviewTexture.height) + ")###Preview";

		userZoom = 1.0f;
		zoomChanged = false;
	}

	if (gPreviewTexture.id == 0) return;

	float previewPosX = io->DisplaySize.x * 0.25f;
	ImVec2 previewWindowSize(io->DisplaySize.x * 0.75f, io->DisplaySize.y - (io->DisplaySize.y * 0.35f));
	ImGui::SetNextWindowPos(ImVec2(previewPosX, gMainMenuBarHeight), ImGuiCond_Always);
	ImGui::SetNextWindowSize(previewWindowSize, ImGuiCond_Always);

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
	std::filesystem::path filePath(node.file->getPath());

	std::string ext = filePath.extension().string();
	if (!ext.empty() && ext[0] == '.')
		ext.erase(0, 1);

	static std::string nameStr;
	static std::string patternStr;

	nameStr = std::string(node.file->getAssetTypeString()) + " | *." + ext + " File";
	patternStr = ext;

	SDL_DialogFileFilter filter{};
	filter.name = nameStr.c_str();
	filter.pattern = patternStr.c_str();

	// this is very dumb and I'll have to give this a rewrite sometime
	if(includeProperties)
		IO::SaveFileDialogWithProperties(GetWindow(), &filter, node.file->getData(), GetFileNameFromPath(node.file->getPath()), true, node.file->getProperties());
	else
		IO::SaveFileDialogWithProperties(GetWindow(), &filter, node.file->getData(), GetFileNameFromPath(node.file->getPath()));
}

static int ShowMessagePrompt(const char* title, const char* message, const SDL_MessageBoxButtonData* buttons, int numButtons)
{
	static SDL_MessageBoxData messageboxdata = {};
	messageboxdata.flags = SDL_MESSAGEBOX_WARNING | SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT;
	messageboxdata.title = title;
	messageboxdata.message = message;
	messageboxdata.numbuttons = numButtons;
	messageboxdata.buttons = buttons;
	messageboxdata.window = GetWindow();

	int buttonID = -1;
	if (SDL_ShowMessageBox(&messageboxdata, &buttonID)) {
		return buttonID; // return the button ID user clicked
	}
	else {
		SDL_Log("Failed to show message box: %s", SDL_GetError());
		return -1; // indicate error
	}
}

static bool ShowYesNoMessagePrompt(const char* title, const char* message)
{
	const static SDL_MessageBoxButtonData buttons[] = {
		{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes" },
		{ SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "No" }
	};

	return ShowMessagePrompt(title, message, buttons, SDL_arraysize(buttons)) == 1;
}

static void SaveFilePropertiesToFile(const FileTreeNode& node, const std::string& outpath)
{
	if (!node.file || outpath.empty())
		return;

	std::u16string propertyData;
	for (const auto& [key, val] : node.file->getProperties()) {
		propertyData += IO::ToUTF16(key) + u' ' + val + u'\n';
	}

	std::ofstream propFile(outpath, std::ios::binary);
	if (propFile.is_open())
	{
		const char* buffer = reinterpret_cast<const char*>(propertyData.data());
		std::size_t byteCount = propertyData.size() * sizeof(char16_t);
		propFile.write(buffer, byteCount);
		propFile.close();
	}
}

static void SaveFilePropertiesAs(const FileTreeNode& node)
{
	static const std::string nameStr = "Text File | *.txt";
	static const std::string patternStr = "txt";

	SDL_DialogFileFilter filter{};
	filter.name = nameStr.c_str();
	filter.pattern = patternStr.c_str();

	std::string outpath = IO::SaveFileDialog(GetWindow(), &filter, GetFileNameFromPath(node.file->getPath()) + ".txt");

	if (!outpath.empty())
		SaveFilePropertiesToFile(node, outpath);
}

static void SaveFolderAsFiles(const FileTreeNode& node, bool includeProperties = false)
{
	std::string targetDir = IO::ChooseFolderDialog(GetWindow(), "Choose Output Directory");
	if (targetDir.empty())
		return;

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
				std::string fileName = GetFileNameFromPath(n.path);
				std::string filePath = currentPath + "/" + fileName;

				std::ofstream outFile(filePath, std::ios::binary);
				if (outFile)
					outFile.write(reinterpret_cast<const char*>(n.file->getData().data()), n.file->getFileSize());

				if (outFile.good() && includeProperties)
				{
					SaveFilePropertiesToFile(n, filePath + ".txt");
				}
			}
		};

	saveRecursive(node, targetDir);
}

static void HandlePCKNodeContextMenu(FileTreeNode& node)
{
	if (ImGui::BeginPopupContextItem()) {
		bool isFile = node.file;

		if (ImGui::BeginMenu("Extract")) {
			if (isFile && ImGui::MenuItem("File"))
			{
				SaveNodeAsFile(node);
			}
			if (!isFile && ImGui::MenuItem("Files"))
			{
				SaveFolderAsFiles(node);
			}
			if (!isFile && ImGui::MenuItem("Files with Properties"))
			{
				SaveFolderAsFiles(node, true);
			}

			bool hasProperties = node.file && !node.file->getProperties().empty();

			if (isFile && hasProperties && ImGui::MenuItem("Properties"))
			{
				SaveFilePropertiesAs(node);
			}

			if (isFile && hasProperties && ImGui::MenuItem("File with Properties"))
			{
				SaveNodeAsFile(node, true);
			}

			ImGui::EndMenu();
		}
		if (isFile && ImGui::BeginMenu("Replace"))
		{
			if (ImGui::MenuItem("File Data"))
			{
				if (SetDataFromFile(*node.file))
					ResetPreviewWindow();
			}
			if (ImGui::MenuItem("File Properties"))
			{
				SetPropertiesFromFile(*node.file);
			}
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Delete")) {
			if (ShowYesNoMessagePrompt("Are you sure?", "This is permanent and cannot be undone.\nIf this is a folder, all sub-files will be deleted too."))
			{
				DeleteNode(node);
			}
		}
		ImGui::EndPopup();
	}
}

bool IsClicked()
{
	return (ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right)) ||
		(ImGui::IsMouseReleased(ImGuiMouseButton_Right) && ImGui::IsItemHovered());
}

// Renders the passed node, also handles children or the node
static void RenderNode(FileTreeNode& node, std::vector<const FileTreeNode*>* visibleList = nullptr) {
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
		if (IsClicked())
			gSelectedPath = node.path;

		HandlePCKNodeContextMenu(node);

		if (open) {
			for (auto& child : node.children)
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

		if (IsClicked())
			gSelectedPath = node.path;

		HandlePCKNodeContextMenu(node);
	}
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
	ImGui::Begin(std::string(gCurrentPCKFileName + "###FileTree").c_str(), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	gShouldOpenFolder = false;
	gShouldCloseFolder = false;

	if (ImGui::IsWindowFocused() && !gSelectedPath.empty()) {
		if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) gShouldOpenFolder = true;
		else if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) gShouldCloseFolder = true;
	}

	for (auto& node : gTreeNodes) RenderNode(node, &gVisibleNodes);

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
	if (selectedFile)
	{
		if (selectedFile->isImageType())
		{
			HandlePreviewWindow(*selectedFile);
		}

		HandlePropertiesWindow(*selectedFile);
	}

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

	ImGuiStyle& style = ImGui::GetStyle();
	style.CellPadding = ImVec2(0, 0);

	io = &ImGui::GetIO();

	ImFontConfig config;
	config.MergeMode = false;
	config.PixelSnapH = true;

	io->Fonts->AddFontFromFileTTF("assets/fonts/m6x11plus.ttf", 18.0f, &config);

	config.MergeMode = true;

	// Merge Chinese (Simplified)
	io->Fonts->AddFontFromFileTTF("assets/fonts/ark-pixel-12px-monospaced-zh_cn.ttf", 18.0f, &config, io->Fonts->GetGlyphRangesChineseSimplifiedCommon());
	// Merge Chinese (Traditional)
	io->Fonts->AddFontFromFileTTF("assets/fonts/ark-pixel-12px-monospaced-zh_tw.ttf", 18.0f, &config, io->Fonts->GetGlyphRangesChineseFull());
	// Merge Japanese
	io->Fonts->AddFontFromFileTTF("assets/fonts/ark-pixel-12px-monospaced-ja.ttf", 18.0f, &config, io->Fonts->GetGlyphRangesJapanese());
	// Merge Korean
	io->Fonts->AddFontFromFileTTF("assets/fonts/ark-pixel-12px-monospaced-ko.ttf", 18.0f, &config, io->Fonts->GetGlyphRangesKorean());

	io->Fonts->Build();
}

void ResetUIData(const std::string& filePath) {

	if (gCurrentPCK)
	{
		gHasXMLSupport = gCurrentPCK->getXMLSupport();
		gPCKEndianness = gCurrentPCK->getEndianness();
	}

	gSelectedPropertyIndex = -1;
	gSelectedPath = "";
	gCurrentPCKFilePath = filePath.empty() ? "" : filePath;
	gCurrentPCKFileName = filePath.empty() ? "" : std::filesystem::path(filePath).filename().string();
	gKeyboardScrolled = false;
	gShouldOpenFolder = false;
	gShouldCloseFolder = false;

	gVisibleNodes.clear();
	gTreeNodes.clear();
}

void UICleanup() {
	ResetUIData();
	for (auto& [type, tex] : gFileIcons)
		if (tex.id != 0)
			glDeleteTextures(1, &tex.id);
	gFileIcons.clear();
}
