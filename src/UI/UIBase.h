#pragma once

#include "PCK/PCKAssetFile.h"
#include "UI/Tree/TreeNode.h"

class UIBase {
public:
	virtual ~UIBase() {}
	// Initializes UI framework
	virtual bool Init() = 0;

	// Processes events in UI framework
	virtual void ProcessEvent(void* event) = 0;

	// Starts new frame in UI framework to render
	virtual void NewFrame() = 0;

	// Renders frame data
	virtual void Render() = 0;

	// Runs the shutdown/clean up event
	virtual void Shutdown() = 0;

	// Renders the file tree in the main program form
	virtual void RenderFileTree() = 0;

	// Renders the menu bar in the window
	virtual void RenderMenuBar() = 0;

	// Renders the context menu in the main program form's file tree
	virtual void RenderContextMenu(FileTreeNode& node) = 0;

	// Renders the preview window in the main program form, takes file to preview
	virtual void RenderPreviewWindow(PCKAssetFile& file) = 0;

	// Renders the properties window in the main program form, takes file to get properties from lol
	virtual void RenderPropertiesWindow(PCKAssetFile& file) = 0;

	// Renders a node on the node tree; TODO: probably clean up the function's parameters. This seems unnecessary.
	virtual void RenderNode(FileTreeNode& node, std::vector<const FileTreeNode*>* visibleList = nullptr, bool shouldScroll = false, bool openFolder = false, bool closeFolder = false) = 0;

	// Shows a modal pop up to handle dropped file types with numerous actions for the same extension
	virtual void ShowFileDropPopUp(const std::string& filepath) = 0;

	// Handles keyboard input. I'm unsure if this really should be here or another class lol
	virtual void HandleInput() = 0;
};