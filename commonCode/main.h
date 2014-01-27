/** @file main.h
 *  @brief common includes, function prototypes for samples
 *
 *	Effects in Games, June 2012
 */

#pragma once

#define _EX_USE_ANTTWEAKBAR
#define _EX_SHOW_CONSOLE

#include <string>
#include <utility>
#include <memory>
#include "gl_comp_3_3.h"
#include "wgl_wgl.h"
#include "freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#ifndef _MSC_VER // anttweakbar only in msvc
    #undef _EX_USE_ANTTWEAKBAR
#endif

#ifdef _EX_USE_ANTTWEAKBAR
    #include <AntTweakBar.h>
#endif

#define WND_START_WIDTH  800
#define WND_START_HEIGHT 600

#ifndef M_PI
    #define M_PI 3.141592
#endif

///////////////////////////////////////////////////////////////////////////////
struct Globals 
{
    static std::string ApplicationWindowName;
    static double AppTime;	// global app time in seconds
    static float Fps;

    static unsigned int MainWindowWidth;
    static unsigned int MainWindowHeight;

#ifdef _EX_USE_ANTTWEAKBAR
    static TwBar *MainTweakBar;
#endif
};

///////////////////////////////////////////////////////////////////////////////
bool initApp();
void cleanUp();

// window:
void changeSize(int w, int h);

// keyboard:
void processNormalKeys(unsigned char key, int x, int y);
void pressSpecialKey(int key, int x, int y);
void releaseSpecialKey(int key, int x, int y);

// mouse:
void processMouse(int button, int state, int x, int y);
void processMouseMotion(int x, int y);
void processMousePassiveMotion(int x, int y);

// update & rendering
void updateScene(double deltaTime);
void renderScene();
