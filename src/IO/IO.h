#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_dialog.h>
#include <algorithm>
#include <fstream>
#include <functional>
#include <string>

namespace IO
{
	enum class Endianness
	{
		LITTLE,
		BIG,
		ENDIANESS_TOTAL
	};

	std::string OpenFile(SDL_Window* window, SDL_DialogFileFilter* filters);
}