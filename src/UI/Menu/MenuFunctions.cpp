#include "MenuFunctions.h"
#include "../UI.h"

SDL_DialogFileFilter pckFilter[] = {
	{ "Minecraft LCE DLC Files (*.pck)", "pck" }
};

void OpenPCKFile()
{
	std::string filePath = IO::OpenFileDialog(GetWindow(), pckFilter);

	PCKFile*& currentPCKFile = GetCurrentPCKFile();

	if (!filePath.empty()) {
		delete currentPCKFile;
		currentPCKFile = new PCKFile();

		try {
			currentPCKFile->Read(filePath);
		}
		catch (const std::exception& e) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", e.what(), GetWindow());
			delete currentPCKFile;
			currentPCKFile = new PCKFile();
			return;
		}
		catch (...) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Unknown Error Occured.", GetWindow());
			delete currentPCKFile;
			currentPCKFile = new PCKFile();
			return;
		}
	}
	else {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "User aborted operation.", GetWindow());
		return;
	}

	// if successful, reset node and UI data; pass file path to send to UI
	ResetUIData(filePath);
}

void SavePCKFileAs(IO::Endianness endianness, const std::string& defaultName)
{
	PCKFile*& currentPCKFile = GetCurrentPCKFile();

	std::string filePath = IO::SaveFileDialog(GetWindow(), pckFilter, defaultName);

	if(!filePath.empty())
		SavePCKFile(filePath, endianness);
}

void SavePCKFile(const std::string& outpath, IO::Endianness endianness)
{
	PCKFile*& currentPCKFile = GetCurrentPCKFile();

	try {
		currentPCKFile->Write(outpath, endianness);
	}
	catch (const std::exception& e) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", e.what(), GetWindow());
		delete currentPCKFile;
		currentPCKFile = new PCKFile();
	}

	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Saved", "File successfully saved!", GetWindow());
}