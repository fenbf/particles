/** @file drawTest.cpp
 *  @brief draw test for particles
 *  @date 22 January 2014
 *  @author Bartlomiej Filipek, bfilipek.com
 *  @licence public domain
 */

#include "main.h"
#include "globals.h"
#include "Init.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "ShaderLoader.h"
#include "TextureLoader.h"
#include "UIWrapper.h"

#include "freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Particles.h"
#include "ParticleUpdaters.h"
#include "particleGenerators.h"
#include "ParticleRenderer.h"
#include "TimeQuery.h"
#include "particleRenderer.h"

#include "effect.h"
#include <iostream>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// globals

std::string Globals::ApplicationWindowName = "Particles Tests";

ShaderProgram mProgram;
IEffect *gCurrentEffect = nullptr;
int gCurrentEffectID = 0;
int gSelectedEffect = 0;

int gNumParticles = 0;
int gNumAlive = 0;

unsigned int gParticleTexture = 0;

struct Camera
{
	float camDistance;
	glm::vec3 cameraDir;
	glm::mat4 modelviewMatrix;
	glm::mat4 projectionMatrix;
} camera;

struct EffectInfo
{
	std::shared_ptr<IEffect> effect;
	CpuTimeQuery cpuParticlesUpdate;
	CpuTimeQuery cpuBuffersUpdate;
	double avgFps = 0.0;
	double totalTime = 0.0;
	double startTime = 0.0;
	unsigned long frameCount = 0;
	GpuTimerQuery gpuUpdate;
	GpuTimerQuery gpuRender;
} effectInfo[3];

double cpuParticlesUpdateTime = 0.0;
double cpuBuffersUpdateTime = 0.0;
double gpuBuffersUpdateTime = 0.0;
double gpuRenderTime = 0.0;

bool gAnimationOn = true;
bool benchmarkMode = false;

const double SKIP_TIME = 2.0;
const double TIME_PER_EFFECT = 5.0;

const char *searchArgv(int argc, char **argv, const char *key, const char *defaultValue)
{
	for (int i = 1; i < argc; ++i)
	{
		char *p = strstr(argv[i], key);
		if (p)
			return p + strlen(key);
	}
	return defaultValue;
}

///////////////////////////////////////////////////////////////////////////////
bool initApp(int argc, char **argv)
{
    //
    // some global GL states
    //
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.15f, 0.15f, 0.15f, 0.0f);

	camera.cameraDir[0] = 0.0f;
	camera.cameraDir[1] = 0.0f;
	camera.cameraDir[2] = 1.0f;
	camera.camDistance = 1.0f;

    //
    // shaders
    //
    if (!shaderLoader::loadAndBuildShaderPairFromFile(&mProgram, "shaders/drawTest.vs", "shaders/drawTest.fs"))
        return false;

	//
	// generate texture:
	//
	if (!textureLoader::loadTexture("data\\particle.png", &gParticleTexture))
		return false;

	mProgram.use();
	mProgram.uniform1i("tex", 0);
	mProgram.disable();

	if (strstr(searchArgv(argc, argv, "benchmark=", ""), "true"))
		benchmarkMode = true;

	const char *RENDERER_NAME = searchArgv(argc, argv, "renderer=", "gl");
	if (!particles::RendererFactory::isAvailable(RENDERER_NAME))
	{
		LOG_ERROR("Unknown renderer name: \"%s\"!", RENDERER_NAME);
		return false;
	}

	effectInfo[0].effect = EffectFactory::create("tunnel");
	effectInfo[0].effect->initialize(IEffect::DEFAULT_PARTICLE_NUM_FLAG);
	if (!effectInfo[0].effect->initializeRenderer(RENDERER_NAME))
	{
		LOG_ERROR("Cannot create \"%s\" Renderer!", RENDERER_NAME);
		return false;
	}
	effectInfo[1].effect = EffectFactory::create("attractors");
	effectInfo[1].effect->initialize(IEffect::DEFAULT_PARTICLE_NUM_FLAG);
	if (!effectInfo[1].effect->initializeRenderer(RENDERER_NAME))
	{
		LOG_ERROR("Cannot create \"%s\" Renderer!", RENDERER_NAME);
		return false;
	}
	effectInfo[2].effect = EffectFactory::create("fountain");
	effectInfo[2].effect->initialize(IEffect::DEFAULT_PARTICLE_NUM_FLAG);
	if (!effectInfo[2].effect->initializeRenderer(RENDERER_NAME))
	{
		LOG_ERROR("Cannot create \"%s\" Renderer!", RENDERER_NAME);
		return false;
	}
	gCurrentEffectID = 0;
	gCurrentEffect = effectInfo[0].effect.get();

	effectInfo[0].gpuUpdate.init();
	effectInfo[1].gpuUpdate.init();
	effectInfo[2].gpuUpdate.init();

	effectInfo[0].gpuRender.init();
	effectInfo[1].gpuRender.init();
	effectInfo[2].gpuRender.init();

	ui::AddTweak("animate", &gAnimationOn, "");
	ui::AddVar("particles", &gNumParticles, "");
	ui::AddVar("alive", &gNumAlive, "");
	ui::AddVar("cpu particles", &cpuParticlesUpdateTime, "precision=3 group=timers");
	ui::AddVar("cpu buffers", &cpuBuffersUpdateTime, "precision=3 group=timers");

	ui::AddVar("gpu buffer", &gpuBuffersUpdateTime, "precision=3 group=timers");
	ui::AddVar("gpu render", &gpuRenderTime, "precision=3 group=timers");		

	ui::AddTweakDir3f("camera", &camera.cameraDir.x, "");
	ui::AddTweak("camera distance", &camera.camDistance, "min=0.05 max=4.0 step=0.01");
	ui::AddSeparator();
	ui::AddTweak("effect id", &gSelectedEffect, "min=0 max=2");
	gCurrentEffect->addUI();
	
	effectInfo[gCurrentEffectID].startTime = Globals::AppTimeInSec;
	
	Globals::ShowUI = false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
void cleanUp()
{
	const std::vector<std::string> EFFECTS_NAME{ "tunnel", "attractors", "fountain" };

	std::cout << "ratio: " << gCurrentEffect->aliveToAllRatio() << std::endl;

	// show average times:
	for (int i = 0; i < 3; ++i)
	{
		std::cout << EFFECTS_NAME[i] << std::endl;
		if (effectInfo[i].frameCount > 10)
		{
			//std::cout << "avg cpu update time: " << effectInfo[i].cpuParticlesUpdate.getAverage() << std::endl;
			//std::cout << "avg cpu buffer time: " << effectInfo[i].cpuBuffersUpdate.getAverage() << std::endl;

			//std::cout << "avg gpu update time: " << effectInfo[i].gpuUpdate.getAverageTime() << std::endl;
			//std::cout << "avg gpu render time: " << effectInfo[i].gpuRender.getAverageTime() << std::endl;
			double totalTime = effectInfo[i].totalTime;
			std::cout << "avg fps:             " << effectInfo[i].avgFps / effectInfo[i].frameCount << std::endl;
			std::cout << "avg fps:             " << effectInfo[i].frameCount / totalTime << std::endl;
			std::cout << "avg time per frame:  " << totalTime / effectInfo[i].frameCount << std::endl;
			std::cout << "time per effect:      " << totalTime << std::endl;
			std::cout << "frames for effect      " << effectInfo[i].frameCount << std::endl;
		}
		else
			std::cout << "no data..." << std::endl;
	}

	gCurrentEffect->clean();
	glDeleteTextures(1, &gParticleTexture);
}

///////////////////////////////////////////////////////////////////////////////
void changeSize(int w, int h)
{
    float aspect = (float)w/(float)h;

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// setup projection matrix
	camera.projectionMatrix = glm::perspective(45.0f, aspect, 0.1f, 1000.0f);
}

///////////////////////////////////////////////////////////////////////////////
void processNormalKeys(unsigned char key, int x, int y) 
{
	if (key == 27)
		glutLeaveMainLoop();
	else if (key == ' ')
		gAnimationOn = !gAnimationOn;
	else if (key == 'r')
		gCurrentEffect->reset();
	else if (key == 'u')
		Globals::ShowUI = !Globals::ShowUI;
	else if (key == 'z')
		gSelectedEffect = (gSelectedEffect - 1) % 3;
	else if (key == 'x')
		gSelectedEffect = (gSelectedEffect + 1) % 3;
}

///////////////////////////////////////////////////////////////////////////////
void pressSpecialKey(int key, int x, int y) 
{
	if (key == GLUT_KEY_UP)
		camera.camDistance += 0.1f;
	else if (key == GLUT_KEY_DOWN)
		camera.camDistance-= 0.1f;
}


///////////////////////////////////////////////////////////////////////////////
void releaseSpecialKey(int key, int x, int y) 
{
	
}

///////////////////////////////////////////////////////////////////////////////
void processMouse(int button, int state, int x, int y)
{
	// todo: add some custom code...
}

///////////////////////////////////////////////////////////////////////////////
void processMouseMotion(int x, int y)
{
	// todo: add some custom code...	
}

///////////////////////////////////////////////////////////////////////////////
void processMousePassiveMotion(int x, int y)
{
	// todo: add some custom code...	
}

void CalcTotalTimeForEffect()
{
	effectInfo[gCurrentEffectID].totalTime += Globals::AppTimeInSec - effectInfo[gCurrentEffectID].startTime - SKIP_TIME;
}

void CalcTotalTimeForEffect();

///////////////////////////////////////////////////////////////////////////////
void updateScene(double deltaTime) 
{
	if (!gAnimationOn)
		return;

	if (benchmarkMode)
	{
		if (Globals::AppTimeInSec - effectInfo[gCurrentEffectID].startTime > TIME_PER_EFFECT+SKIP_TIME)
		{
			CalcTotalTimeForEffect();
			if (gCurrentEffectID == 0)
				gSelectedEffect = 1;
			else if (gCurrentEffectID == 1)
				gSelectedEffect = 2;
			else if (gSelectedEffect == 2)
				glutLeaveMainLoop();
		}
	}

	if (gSelectedEffect != gCurrentEffectID)
	{
		std::cout << "ratio: " << gCurrentEffect->aliveToAllRatio() << std::endl;
		gCurrentEffect->removeUI();
		gCurrentEffectID = gSelectedEffect;
		gCurrentEffect = effectInfo[gCurrentEffectID].effect.get();
		gCurrentEffect->addUI();		

		effectInfo[gCurrentEffectID].startTime = Globals::AppTimeInSec;
	}

	gCurrentEffect->update(deltaTime);

	effectInfo[gCurrentEffectID].cpuParticlesUpdate.begin();
		gCurrentEffect->cpuUpdate(deltaTime);
		gNumAlive = 0;// gParticleSystem->numAliveParticles();
	effectInfo[gCurrentEffectID].cpuParticlesUpdate.end();

	effectInfo[gCurrentEffectID].cpuBuffersUpdate.begin();
		//effectInfo[gCurrentEffectID].gpuUpdate.begin();
			gCurrentEffect->gpuUpdate(deltaTime);
		//effectInfo[gCurrentEffectID].gpuUpdate.end();
	effectInfo[gCurrentEffectID].cpuBuffersUpdate.end();

	cpuParticlesUpdateTime = effectInfo[gCurrentEffectID].cpuParticlesUpdate.m_time;
	cpuBuffersUpdateTime = effectInfo[gCurrentEffectID].cpuBuffersUpdate.m_time;

	gNumParticles = gCurrentEffect->numAllParticles();
	gNumAlive = gCurrentEffect->numAliveParticles();

	if (Globals::AppTimeInSec - effectInfo[gCurrentEffectID].startTime > SKIP_TIME)
	{
		effectInfo[gCurrentEffectID].avgFps += Globals::Fps;
		effectInfo[gCurrentEffectID].frameCount++;
	}

	//effectInfo[gCurrentEffectID].gpuUpdate.updateResults(GpuTimerQuery::WaitOption::WaitForResults);
	//gpuBuffersUpdateTime = effectInfo[gCurrentEffectID].gpuUpdate.getTime();
}

///////////////////////////////////////////////////////////////////////////////
void renderScene() 
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(FALSE);

	//
	// camera
	//
	camera.modelviewMatrix = glm::lookAt(camera.cameraDir*camera.camDistance, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, gParticleTexture);

	//glEnable(GL_PROGRAM_POINT_SIZE);

	mProgram.use();
	mProgram.uniformMatrix4f("matProjection", glm::value_ptr(camera.projectionMatrix));
	mProgram.uniformMatrix4f("matModelview", glm::value_ptr(camera.modelviewMatrix));

	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	//effectInfo[gCurrentEffectID].gpuRender.begin();
		gCurrentEffect->render();
	//effectInfo[gCurrentEffectID].gpuRender.end();

	glDisable(GL_BLEND);

    mProgram.disable();
	
	//effectInfo[gCurrentEffectID].gpuRender.updateResults(GpuTimerQuery::WaitOption::WaitForResults);
	//gpuRenderTime = effectInfo[gCurrentEffectID].gpuRender.getTime();
}
