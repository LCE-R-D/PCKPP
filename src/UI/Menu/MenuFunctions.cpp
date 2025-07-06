#include "MenuFunctions.h"
#include "../UI.h"

void OpenPCKFile(SDL_Window* window)
{
	SDL_DialogFileFilter filters[] = {
		{ "Minecraft LCE DLC Files (*.pck)", "pck" }
	};

	std::string filePath = IO::OpenFile(window, filters);

	PCKFile*& currentPCKFile = GetCurrentPCKFile();

	if (!filePath.empty()) {
		delete currentPCKFile;
		currentPCKFile = new PCKFile();

		try {
			currentPCKFile->Read(filePath);
		}
		catch (const std::exception& e) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", e.what(), window);
			delete currentPCKFile;
			currentPCKFile = new PCKFile();
		}
	}
	else {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "User aborted operation.", window);
	}

	// if successful, reset node and UI data
	ResetUIData();
}