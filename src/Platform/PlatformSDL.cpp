#include <mutex>
#include <condition_variable>
#include "Application/Application.h"
#include "Platform/PlatformSDL.h"

PlatformSDL::~PlatformSDL() {
	Shutdown();
}

bool PlatformSDL::Init(const char* title, int width, int height) {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
		return false;
	}

	std::filesystem::current_path(SDL_GetBasePath());

	mWindow = SDL_CreateWindow(title, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!mWindow) {
		std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
		return false;
	}

	mGLContext = SDL_GL_CreateContext(mWindow);
	if (!SDL_GL_SetSwapInterval(1)) {
		return false;
	}

	if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2))
		return false;
	if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1))
		return false;

	if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY))
		return false;

	if (SDL_GL_MakeCurrent(mWindow, mGLContext) < 0) {
		std::cerr << "SDL_GL_MakeCurrent failed: " << SDL_GetError() << "\n";
		return false;
	}

	// Sets the correct icon
	SDL_Surface* icon = SDL_LoadBMP("assets/icons/ICON_PCKPP.bmp");
	if (icon) {
		SDL_SetWindowIcon(GetWindow(), icon);
		SDL_DestroySurface(icon);
	}

	return !mShouldClose;
}

void PlatformSDL::PollEvents(PlatformBackend* backend) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (backend)
			backend->ProcessEvent(&event);

		switch (event.type)
		{
			case SDL_EVENT_QUIT:
				mShouldClose = true;
				break;
			case SDL_EVENT_DROP_FILE:
				gApp->GetUI()->ShowFileDropPopUp(event.drop.data);
				break;
			default:
				break;
		}
	}
}

bool PlatformSDL::ShouldClose() const {
	return mShouldClose;
}

void PlatformSDL::Shutdown() {
	if (mWindow) {
		SDL_DestroyWindow(mWindow);
		mWindow = nullptr;
	}
	SDL_Quit();
}

// TODO: Change this because I hate the choose dialog thing for Windows
std::string PlatformSDL::FileDialog::ChooseFolder() {
	return ShowDialog(SDL_FILEDIALOG_OPENFOLDER, {}, "");
}

std::string PlatformSDL::FileDialog::OpenFile(const std::vector<FileFilter>& filters) {
	return ShowDialog(SDL_FILEDIALOG_OPENFILE, filters, "");
}

std::string PlatformSDL::FileDialog::SaveFile(const std::vector<FileFilter>& filters, const std::string& defaultName) {
	return ShowDialog(SDL_FILEDIALOG_SAVEFILE, filters, defaultName);
}

std::mutex gMutex;
bool gDialogFinished;
std::condition_variable gConditionVariable;
std::string gSelectedPath;

std::string PlatformSDL::FileDialog::ShowDialog(SDL_FileDialogType type, const std::vector<FileFilter>& filters, const std::string& defaultName) {
	gDialogFinished = false;
	gSelectedPath.clear();
	SDL_PropertiesID props = SDL_CreateProperties();

	std::vector<SDL_DialogFileFilter> sdlFilters;

	for (const auto& f : filters) {
		sdlFilters.push_back({
			f.label.c_str(),
			f.pattern.c_str()
			});
	}

	if (!sdlFilters.empty()) {
		SDL_SetPointerProperty(props, SDL_PROP_FILE_DIALOG_FILTERS_POINTER, sdlFilters.data());
		SDL_SetNumberProperty(props, SDL_PROP_FILE_DIALOG_NFILTERS_NUMBER, static_cast<int>(sdlFilters.size()));
	}

	SDL_SetPointerProperty(props, SDL_PROP_FILE_DIALOG_WINDOW_POINTER, mParent.mWindow);
	SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_TITLE_STRING,
		(type == SDL_FILEDIALOG_SAVEFILE) ? "Save File" :
		(type == SDL_FILEDIALOG_OPENFOLDER) ? "Choose Folder" : "Open File");
	SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_ACCEPT_STRING, "OK");
	SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_CANCEL_STRING, "Cancel");

	if (!defaultName.empty()) {
		SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_LOCATION_STRING, defaultName.c_str());
	}

	auto callback = [](void* userdata, const char* const* filelist, int count) {
		std::lock_guard<std::mutex> lock(gMutex);

		if (filelist && *filelist)
			gSelectedPath = *filelist;
		else
			gSelectedPath.clear();

		gDialogFinished = true;
		gConditionVariable.notify_one();
		};

	SDL_ShowFileDialogWithProperties(type, callback, nullptr, props);

	SDL_Event event;
	while (!gDialogFinished)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT) {
				gDialogFinished = true;
			}
		}

		std::unique_lock<std::mutex> lock(gMutex);
		gConditionVariable.wait_for(lock, std::chrono::milliseconds(10), [] { return gDialogFinished; });
	}

	SDL_Log("%s", gSelectedPath.c_str());

	SDL_DestroyProperties(props);

	return gSelectedPath;
}

void PlatformSDL::FileDialog::ShowError(const std::string& title, const std::string& message) {
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), message.c_str(), mParent.mWindow);
}

void PlatformSDL::FileDialog::ShowInfo(const std::string& title, const std::string& message) {
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title.c_str(), message.c_str(), mParent.mWindow);
}

int PlatformSDL::ShowMessagePrompt(const char* title, const char* message,
	const std::vector<FileDialogBase::MessageButton>& buttons)
{
	std::vector<SDL_MessageBoxButtonData> sdlButtons;
	sdlButtons.reserve(buttons.size());

	for (const auto& b : buttons) {
		SDL_MessageBoxButtonData buttonData = {};
		buttonData.flags = b.flags;
		buttonData.buttonID = b.id;
		buttonData.text = b.text.c_str();

		sdlButtons.push_back(buttonData);
	}

	SDL_MessageBoxData messageboxdata = {};
	messageboxdata.flags = SDL_MESSAGEBOX_WARNING | SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT;
	messageboxdata.title = title;
	messageboxdata.message = message;
	messageboxdata.numbuttons = static_cast<int>(sdlButtons.size());
	messageboxdata.buttons = sdlButtons.data();
	messageboxdata.window = mWindow;

	int buttonID = -1;
	if (SDL_ShowMessageBox(&messageboxdata, &buttonID)) {
		return buttonID;
	}
	else {
		SDL_Log("Failed to show message box: %s", SDL_GetError());
		return -1;
	}
}

void PlatformSDL::ShowSuccessMessage() {
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Success", "Operation performed successfully.", mWindow);
}

void PlatformSDL::ShowCancelledMessage() {
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Cancelled", "User aborted operation.", mWindow);
}

bool PlatformSDL::ShowYesNoMessagePrompt(const char* title, const char* message) {
	static const std::vector<FileDialogBase::MessageButton> buttons = {
		{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes" },
		{ SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "No" }
	};

	int result = ShowMessagePrompt(title, message, buttons);
	return result == 1;
}