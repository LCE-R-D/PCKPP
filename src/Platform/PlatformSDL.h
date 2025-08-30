#pragma once

#include <SDL3/SDL.h>
#include "Platform/PlatformBase.h"

class PlatformSDL : public PlatformBase {
public:
	// Initialize dialog member
	PlatformSDL() : mDialog(*this) {}
	~PlatformSDL();

	// Initialze SDL
	bool Init(const char* title, int width, int height) override;

	// Process SDL events
	void PollEvents(PlatformBackend* backend) override;

	// Check if application should close
	bool ShouldClose() const override;

	// Run the SDL shutdown/clean up event
	void Shutdown() override;

	// Show basic message prompt, extendable with buttons
	int ShowMessagePrompt(const char* title, const char* message,
		const std::vector<FileDialogBase::MessageButton>& buttons) override;

	// Show common success message prompt
	void ShowSuccessMessage() override;

	// Show common cancellation message prompt
	void ShowCancelledMessage() override;

	// Show common yes/no message prompt
	bool ShowYesNoMessagePrompt(const char* title, const char* message) override;

	// Gets the SDL window
	SDL_Window* GetWindow() const { return mWindow; };

	// SDL FileDialog stuff
	class FileDialog : public FileDialogBase {
	public:
		// only constructor with a given parent
		explicit FileDialog(PlatformSDL& parent) : mParent(parent) {}

		// Open file dialog from disk
		std::string OpenFile(const std::vector<FileFilter>& filters) override;

		// Save file dialog to disk
		std::string SaveFile(const std::vector<FileFilter>& filters, const std::string& defaultName = "") override;

		// Choose file dialog from disk
		std::string ChooseFolder() override;

		// Show common error prompt
		void ShowError(const std::string& title, const std::string& message) override;

		// Show common error information prompt
		void ShowInfo(const std::string& title, const std::string& message) override;

	private:
		PlatformSDL& mParent;
		std::string ShowDialog(SDL_FileDialogType type, const std::vector<FileFilter>& filters, const std::string& defaultName);
	};

	// File dialog object
	FileDialog mDialog;

private:
	SDL_Window* mWindow = nullptr;
	SDL_GLContext mGLContext = nullptr;
	bool mShouldClose = false;
};