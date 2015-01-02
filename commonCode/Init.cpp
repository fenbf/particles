/** @file init.cpp
*  @brief impleentation of useful utilities for opengl based apps
*
*	@author Bartlomiej Filipek 
*/

#include "commonCode.h"
#include "init.h"
#include "Log.h"
#include "freeglut.h"


void APIENTRY DebugFunc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
						const GLchar* message, const GLvoid* userParam)
{
	std::string srcName;
	switch(source)
	{
		case GL_DEBUG_SOURCE_API: srcName = "API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: srcName = "Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: srcName = "Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: srcName = "Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION: srcName = "Application"; break;
		case GL_DEBUG_SOURCE_OTHER: srcName = "Other"; break;
	}

	std::string errorType;
	switch(type)
	{
		case GL_DEBUG_TYPE_ERROR: errorType = "Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: errorType = "Deprecated Functionality"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: errorType = "Undefined Behavior"; break;
		case GL_DEBUG_TYPE_PORTABILITY: errorType = "Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE: errorType = "Performance"; break;
		case GL_DEBUG_TYPE_OTHER: errorType = "Other"; break;
	}

	std::string typeSeverity;
	switch(severity)
	{
		case GL_DEBUG_SEVERITY_HIGH: typeSeverity = "High"; break;
		case GL_DEBUG_SEVERITY_MEDIUM: typeSeverity = "Medium"; break;
		case GL_DEBUG_SEVERITY_LOW: typeSeverity = "Low"; break;
	}

	LOG("%s from %s,\t%s priority\nMessage: %s\n",
		errorType.c_str(), srcName.c_str(), typeSeverity.c_str(), message);
}

namespace utils
{
    bool initGL(bool vsync)
    {
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			LOG("GLEW Loaded!");
        }

        if (GLEW_ARB_debug_output)
	    {
    		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	    	glDebugMessageCallback((GLDEBUGPROCARB)DebugFunc, nullptr);
            LOG("Debug Message Callback turned on!");
        }
       // else
        //    LOG("NO Debug Message Callback ;(");			

        // print some GL info:
        int majorVersion = -1;
        int minorVersion = -1;
        
        glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
        glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
        LOG("- - - - - - - - - - - - - - - - - - - - - - - ");
        LOG("OpenGL Version:  %d:%d", majorVersion, minorVersion);
        LOG("OpenGL Renderer: %s", (char *)glGetString(GL_RENDERER));
        LOG("OpenGL Vendor:   %s", (char *)glGetString(GL_VENDOR));
        LOG("GLSL Version:    %s", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
        LOG("- - - - - - - - - - - - - - - - - - - - - - - \n");

#ifdef WIN32
        if (WGLEW_EXT_swap_control)
            wglSwapIntervalEXT(vsync ? 1 : 0);
#endif

        return true;
    }

    void calculateFps(float *fps)
    {
        static unsigned int frame = 0;
        static int timeBase = 0;

        frame++;

        int t = glutGet(GLUT_ELAPSED_TIME);
        if (t - timeBase > 1000) 
        {
            *fps = 0.5f*(*fps) + 0.5f*(frame*1000.0f/(float)(t - timeBase));
            timeBase = t;		
            frame = 0;
        }

    }

    void updateTimer(double *deltaTime, double *appTime, const double MAX_REFRESH_TIME)
    {
        static double lastDeltas[3] = { 0.0, 0.0, 0.0 };

        // in milisec
        int t = glutGet(GLUT_ELAPSED_TIME);
        double newTime = (double)t*0.001;

        *deltaTime = newTime - *appTime;
        if ((*deltaTime) < 0) *deltaTime = 0.0;
        if ((*deltaTime) > MAX_REFRESH_TIME) *deltaTime = MAX_REFRESH_TIME;

        // average:
        *deltaTime = ((*deltaTime) + lastDeltas[0] + lastDeltas[1] + lastDeltas[2]) * 0.25;

        *appTime = (*appTime) + (*deltaTime);

        // save delta:
        lastDeltas[0] = lastDeltas[1];
        lastDeltas[1] = lastDeltas[2];
        lastDeltas[2] = *deltaTime;
    }

    bool checkGLErrors(const char *func, const char * file, int line)
    {
        GLenum err;
        bool good = true;

        err = glGetError();
        while (err != GL_NO_ERROR)
        {
            const char * message = nullptr;
            switch( err )
            {
            case GL_INVALID_ENUM:
                message = "Invalid enum";
                break;
            case GL_INVALID_VALUE:
                message = "Invalid value";
                break;
            case GL_INVALID_OPERATION:
                message = "Invalid operation";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                message = "Invalid framebuffer operation";
                break;
            case GL_OUT_OF_MEMORY:
                message = "Out of memory";
                break;
            default:
                message = "Unknown error";
            }

            LOG("OpenGL ERROR in [%s] %s(%d): %s\n", func, logger::fileNameFromPath(file), line, message);
            good = false;
            err = glGetError();
        }
        return good;
    }
} // namespace utils