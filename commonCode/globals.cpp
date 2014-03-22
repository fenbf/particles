#include "globals.h"

#ifdef _EX_USE_ANTTWEAKBAR
	#include <AntTweakBar.h>
#endif

double Globals::AppTime = 0.0;	// global app time in seconds
float Globals::Fps = 0.0f;

unsigned int Globals::MainWindowWidth = WND_START_WIDTH;
unsigned int Globals::MainWindowHeight = WND_START_HEIGHT;

#ifdef _EX_USE_ANTTWEAKBAR
TwBar *Globals::MainTweakBar = NULL;
#endif