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
#include "Particles.h"
#include "ParticleRenderer.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// globals

std::string Globals::ApplicationWindowName = "Draw Test Particles";
ShaderProgram mProgram;
std::shared_ptr<ParticleSystem> gParticleSystem;
std::shared_ptr<GLParticleRenderer> gParticleRenderer;

void createVertexBuffers()
{    

}

///////////////////////////////////////////////////////////////////////////////

bool initApp() 
{
    //
    // some global GL states
    //
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.15f, 0.15f, 0.15f, 0.0f);

    //
    // shaders
    //
    if (!shaderLoader::loadAndBuildShaderPairFromFile(&mProgram, "shaders/drawTest.vs", "shaders/drawTest.fs"))
        return false;

    createVertexBuffers();

	const size_t NUM_PARTICLES = 10000;
	gParticleSystem = std::make_shared<ParticleSystem>(NUM_PARTICLES);

	auto particleEmitter = std::make_shared<BasicParticleEmitter>(0, NUM_PARTICLES);
	particleEmitter->m_emitRate = 100.0;
	particleEmitter->m_pos = Vec4d{ 0.0 };
	particleEmitter->m_maxStartPosOffset = Vec4d{ 1.0 };
	particleEmitter->m_minStartCol = Vec4d{ 0.0 };
	particleEmitter->m_maxStartCol = Vec4d{ 1.0, 1.0, 1.0, 1.0 };
	particleEmitter->m_minEndCol = Vec4d{ 0.0 };
	particleEmitter->m_maxEndCol = Vec4d{ 1.0, 1.0, 1.0, 0.0 };
	particleEmitter->m_minTime = 0.0;
	particleEmitter->m_maxTime = 2.0;
	particleEmitter->m_minStartVel = Vec4d{ 0.0 };
	particleEmitter->m_maxStartVel = Vec4d{ 0.5, 1.0, 0.5, 0.0 };
	gParticleSystem->addEmitter(particleEmitter);

	auto timeUpdater = std::make_shared<BasicTimeParticleUpdater>(0, NUM_PARTICLES);
	gParticleSystem->addEmitter(timeUpdater);

	auto colorUpdater = std::make_shared<BasicColorParticleUpdater>(0, NUM_PARTICLES);
	gParticleSystem->addEmitter(colorUpdater);

	auto eulerUpdater = std::make_shared<EulerParticleUpdater>(0, NUM_PARTICLES);
	gParticleSystem->addEmitter(eulerUpdater);

	gParticleRenderer = std::make_shared<GLParticleRenderer>();
	gParticleRenderer->generate(gParticleSystem.get(), false);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
void cleanUp()
{
	gParticleRenderer->destroy();
}

///////////////////////////////////////////////////////////////////////////////
void changeSize(int w, int h)
{
    float aspect = (float)w/(float)h;

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);
}

///////////////////////////////////////////////////////////////////////////////
void processNormalKeys(unsigned char key, int x, int y) 
{
	if (key == 27) 
		exit(0);
}

///////////////////////////////////////////////////////////////////////////////
void pressSpecialKey(int key, int x, int y) 
{
 //   if (key == GLUT_KEY_UP)
 //       gCamPos[2] += 1;
 //   else if (key == GLUT_KEY_DOWN)
 //       gCamPos[2] -= 1;

	//else if (key == GLUT_KEY_LEFT)
 //       gCamPos[0] += 0.1f;
 //   else if (key == GLUT_KEY_RIGHT)
 //       gCamPos[0] -= 0.1f;
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
	gParticleSystem->update((FPType)deltaTime);
	gParticleRenderer->update();
}

///////////////////////////////////////////////////////////////////////////////
void renderScene() 
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mProgram.use();
   
	gParticleRenderer->render();

    mProgram.disable();
}
