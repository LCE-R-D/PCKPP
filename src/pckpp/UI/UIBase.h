#pragma once

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
};