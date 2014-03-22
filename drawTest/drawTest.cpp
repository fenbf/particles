/** @file drawTest.cpp
 *  @brief draw test for particles
 *  @date 22 January 2014
 *  @author Bartlomiej Filipek, bfilipek.com
 *  @licence public domain
 */

#include "main.h"
#include "Init.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "ShaderLoader.h"
#include "TextureLoader.h"
#include "UIWrapper.h"

#include "Particles.h"
#include "ParticleUpdaters.h"
#include "particleGenerators.h"
#include "ParticleRenderer.h"
#include "TimeQuery.h"

#include "effect.h"
#include "effectFactory.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// globals

std::string Globals::ApplicationWindowName = "Particles Tests";

ShaderProgram mProgram;
std::shared_ptr<IEffect> gEffects[3];
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

CpuTimeQuery cpuParticlesUpdate;
CpuTimeQuery cpuBuffersUpdate;
GpuTimerQuery gpuUpdate;
double gpuUpdateTime = 0;
GpuTimerQuery gpuRender;
double gpuRenderTime = 0;

bool gAnimationOn = true;

///////////////////////////////////////////////////////////////////////////////
bool initApp() 
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

	gEffects[0] = EffectFactory::create("tunnel");
	gEffects[0]->initialize();
	gEffects[0]->initializeRenderer();
	gEffects[1] = EffectFactory::create("attractors");
	gEffects[1]->initialize();
	gEffects[1]->initializeRenderer();
	gEffects[2] = EffectFactory::create("fountain");
	gEffects[2]->initialize();
	gEffects[2]->initializeRenderer();
	gCurrentEffectID = 0;
	gCurrentEffect = gEffects[0].get();

	gpuUpdate.init();
	gpuRender.init();

	ui::AddTweak("animate", &gAnimationOn, "");
	ui::AddVar("particles", &gNumParticles, "");
	ui::AddVar("alive", &gNumAlive, "");
	ui::AddVar("cpu particles", &cpuParticlesUpdate.m_time, "precision=3 group=timers");
	ui::AddVar("cpu buffers", &cpuBuffersUpdate.m_time, "precision=3 group=timers");

	ui::AddVar("gpu buffer", &gpuUpdate.getTime(), "precision=3 group=timers");
	ui::AddVar("gpu render", &gpuRender.getTime(), "precision=3 group=timers");		

	ui::AddTweakDir3f("camera", &camera.cameraDir.x, "");
	ui::AddTweak("camera distance", &camera.camDistance, "min=0.05 max=4.0 step=0.01");
	ui::AddSeparator();
	ui::AddTweak("effect id", &gSelectedEffect, "min=0 max=2");
	gCurrentEffect->addUI();
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
void cleanUp()
{
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
		exit(0);
	else if (key == ' ')
		gAnimationOn = !gAnimationOn;
	else if (key == 'r')
		gCurrentEffect->reset();
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

///////////////////////////////////////////////////////////////////////////////
void updateScene(double deltaTime) 
{
	if (!gAnimationOn)
		return;

	if (gSelectedEffect != gCurrentEffectID)
	{
		gCurrentEffect->removeUI();
		gCurrentEffectID = gSelectedEffect;
		gCurrentEffect = gEffects[gCurrentEffectID].get();
		gCurrentEffect->addUI();
	}

	gCurrentEffect->update(deltaTime);

	cpuParticlesUpdate.begin();
		gCurrentEffect->cpuUpdate(deltaTime);
		gNumAlive = 0;// gParticleSystem->numAliveParticles();
	cpuParticlesUpdate.end();

	cpuBuffersUpdate.begin();
		gpuUpdate.begin();
			gCurrentEffect->gpuUpdate(deltaTime);
		gpuUpdate.end();		
	cpuBuffersUpdate.end();
	gpuUpdate.updateResults(GpuTimerQuery::WaitOption::WaitForResults);

	gNumParticles = gCurrentEffect->numAllParticles();
	gNumAlive = gCurrentEffect->numAliveParticles();
}

///////////////////////////////////////////////////////////////////////////////
void renderScene() 
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	//
	// camera
	//
	camera.modelviewMatrix = glm::lookAt(camera.cameraDir*camera.camDistance, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gParticleTexture);

	glEnable(GL_PROGRAM_POINT_SIZE);

	mProgram.use();
	mProgram.uniformMatrix4f("matProjection", glm::value_ptr(camera.projectionMatrix));
	mProgram.uniformMatrix4f("matModelview", glm::value_ptr(camera.modelviewMatrix));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	gpuRender.begin();
		gCurrentEffect->render();
	gpuRender.end();

	glDisable(GL_BLEND);

    mProgram.disable();

	gpuRender.updateResults(GpuTimerQuery::WaitOption::WaitForResults);
}
