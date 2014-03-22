/** @file main.cpp
 *  @brief core code for the sample, init, freeGlut setup, etc, etc
 *
 *	Effects in Games, June 2012
 */

#include <ctime>
#include "main.h"
#include "Init.h"
#include "Log.h"


#ifndef _EX_SHOW_CONSOLE
	#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#endif

#define USE_ONLY_CORE_OPENGL_PROFILE

using namespace std;

///////////////////////////////////////////////////////////////////////////////
bool initApp();
void cleanUp();

// window:
void mainChangeSize(int w, int h);

// mouse:
void mainProcessMouse(int button, int state, int x, int y);
void mainProcessMouseMotion(int x, int y);
void mainProcessMousePassiveMotion(int x, int y);

// update & rendering
void mainIdle();

///////////////////////////////////////////////////////////////////////////////
// entry point
int main(int argc, char **argv)
{
	//
	// init GLUT
	//
	glutInit(&argc, argv);

#ifdef USE_ONLY_CORE_OPENGL_PROFILE
	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);
#endif

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200,200);
	glutInitWindowSize(WND_START_WIDTH, WND_START_HEIGHT);
	// glutMainLoop returns and we can release our resources
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    if (glutCreateWindow(Globals::ApplicationWindowName.c_str()) < 1)
	{
		LOG_ERROR("Cannot create the main GLUT window!");
		return 1;
	}

	// set callbacks:
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(pressSpecialKey);
	glutSpecialUpFunc(releaseSpecialKey);
	glutMouseFunc(mainProcessMouse);
	glutMotionFunc(mainProcessMouseMotion);
	glutPassiveMotionFunc(mainProcessMousePassiveMotion);
	glutDisplayFunc(renderScene);
	glutIdleFunc(mainIdle);
	glutReshapeFunc(mainChangeSize);


    // random:
    srand((unsigned int)time(NULL));   


	//
	// 'init' OpenGL: GLEW... and print some basic info
	//
	if (utils::initGL(true) == false) 
		return 1;
	
#ifdef _EX_USE_ANTTWEAKBAR
    #ifdef USE_ONLY_CORE_OPENGL_PROFILE
        TwInit(TW_OPENGL_CORE, NULL);
    #else
	    TwInit(TW_OPENGL, NULL);
    #endif
        Globals::MainTweakBar = TwNewBar("TweakBar");
        TwAddVarRO(Globals::MainTweakBar, "FPS", TW_TYPE_FLOAT, &Globals::Fps, NULL);
#endif

	//
	// init application and run main loop:
	//
    if (initApp())
    {
	    glutMainLoop();
    }
    else
    {
        LOG_ERROR("Cannot init application... end...");
        logger::presentLastMessagesToUser();
	}

	//
	// finish:
	//
	cleanUp();

#ifdef _EX_USE_ANTTWEAKBAR
	TwTerminate();
#endif

	return(0);
}


#pragma region input

///////////////////////////////////////////////////////////////////////////////
void mainChangeSize(int w, int h)
{
    Globals::MainWindowWidth = w;
    Globals::MainWindowHeight = h;		

    changeSize(w, h);

#ifdef _EX_USE_ANTTWEAKBAR
	// Send the new window size to AntTweakBar
    TwWindowSize(w, h);
#endif
}

///////////////////////////////////////////////////////////////////////////////
void mainProcessMouse(int button, int state, int x, int y)
{
	processMouse(button, state, x, y);

#ifdef _EX_USE_ANTTWEAKBAR
	// send message to antTweakBar
	TwEventMouseButtonGLUT(button, state, x, y);
#endif
}

///////////////////////////////////////////////////////////////////////////////
void mainProcessMouseMotion(int x, int y)
{
	processMouseMotion(x, y);
	
#ifdef _EX_USE_ANTTWEAKBAR
	// send message to antTweakBar
	TwEventMouseMotionGLUT(x, y);
#endif
}

///////////////////////////////////////////////////////////////////////////////
void mainProcessMousePassiveMotion(int x, int y)
{
	processMousePassiveMotion(x, y);
	
#ifdef _EX_USE_ANTTWEAKBAR
	// send message to antTweakBar
	TwEventMouseMotionGLUT(x, y);
#endif
}

#pragma endregion

#pragma region update & render

///////////////////////////////////////////////////////////////////////////////
void mainIdle()
{
	double deltaTime;
    utils::updateTimer(&deltaTime, &Globals::AppTime);
	
    utils::calculateFps(&Globals::Fps);

	// call Update:
	updateScene(deltaTime);

	// render frame:
	renderScene();

#ifdef _EX_USE_ANTTWEAKBAR
	TwDraw();
#endif

	glutSwapBuffers();
}

void errIdle()
{
#ifdef _EX_USE_ANTTWEAKBAR
	TwDraw();
#endif

	glutSwapBuffers();
}

#pragma endregion
