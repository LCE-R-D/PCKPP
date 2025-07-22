#pragma once

class RendererBackend {
public:
	virtual ~RendererBackend() = default;

	// Inits the renererer backend
	virtual bool Init(void* rendererData) = 0;

	// Runs the render event
	virtual void Render() = 0;

	// Runs the new frame event
	virtual void NewFrame() = 0;

	// Runs the shut down / cleanup event
	virtual void Shutdown() = 0;
};