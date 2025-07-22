#pragma once

class PlatformBackend {
public:
	virtual ~PlatformBackend() = default;

	// Inits the platform backend
	virtual bool Init(void* platformData) = 0;

	// Even for processing events through backend
	virtual void ProcessEvent(void* event) = 0;

	// Runs the new frame event
	virtual void NewFrame() = 0;

	// Runs the shut down / cleanup event
	virtual void Shutdown() = 0;
};