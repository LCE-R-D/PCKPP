#include "IO.h"
#include <SDL3/SDL_log.h>
#include <mutex>
#include <condition_variable>
#include <atomic>

static std::mutex gMutex;
static std::condition_variable gCV;
static std::string gSelectedFile;
static std::atomic<bool> gDialogFinished(false);

void FileDialogCallback(void* userdata, const char* const* filelist, int filterIndex)
{
	{
		std::lock_guard<std::mutex> lock(gMutex);
		if (filelist && *filelist) {
			gSelectedFile = *filelist;
		}
		else {
			gSelectedFile.clear();
		}
	}
	gDialogFinished = true;
	gCV.notify_one();
}

std::string IO::OpenFile(SDL_Window* window, SDL_DialogFileFilter* filters)
{
	gDialogFinished = false;
	gSelectedFile.clear();

	SDL_ShowOpenFileDialog(FileDialogCallback, nullptr, window, filters, sizeof(*filters)/sizeof(filters[0]), nullptr, false);

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
		gCV.wait_for(lock, std::chrono::milliseconds(10), [] { return gDialogFinished.load(); });
	}

	return gSelectedFile;
}