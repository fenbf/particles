#pragma once

#include <string>

#define WND_START_WIDTH  1280
#define WND_START_HEIGHT 720

struct Globals
{
	static std::string ApplicationWindowName;
	static double AppTime;	// global app time in seconds
	static float Fps;

	static unsigned int MainWindowWidth;
	static unsigned int MainWindowHeight;

#ifdef _EX_USE_ANTTWEAKBAR
	typedef struct CTwBar TwBar;
	static TwBar *MainTweakBar;
#endif
};