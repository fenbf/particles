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

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// globals

std::string Globals::ApplicationWindowName = "Draw Test Particles";

ShaderProgram mProgram;
std::shared_ptr<ParticleSystem> gParticleSystem;
std::shared_ptr<GLParticleRenderer> gParticleRenderer;
std::shared_ptr<particleGenerators::RoundPosGen> gPosGenerator;
std::shared_ptr<particleGenerators::BasicColorGen> gColGenerator;
int gNumParticles = 0;
int gNumAlive = 0;

unsigned int gParticleTexture = 0;

struct Camera
{
	glm::vec3 cameraPosition;
	glm::mat4 modelviewMatrix;
	glm::mat4 projectionMatrix;
} camera;

class CpuTimeQuery
{
public:
	double m_time;
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> cpuTimePointStart;

public:
	void begin()
	{
		cpuTimePointStart = std::chrono::high_resolution_clock::now();
	}

	void end()
	{
		auto diff = std::chrono::high_resolution_clock::now() - cpuTimePointStart;
		auto mili = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
		m_time = 0.5*(m_time + (double)mili);
	}
};

CpuTimeQuery cpuParticlesUpdate;
CpuTimeQuery cpuBuffersUpdate;
TimerQuery gpuUpdate;
double gpuUpdateTime = 0;
TimerQuery gpuRender;
double gpuRenderTime = 0;

///////////////////////////////////////////////////////////////////////////////
bool initApp() 
{
    //
    // some global GL states
    //
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.15f, 0.15f, 0.15f, 0.0f);

	camera.cameraPosition[0] = 0.0f;
	camera.cameraPosition[1] = 0.0f;
	camera.cameraPosition[2] = 1.0f;

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

	//
	// particles
	//
	const size_t NUM_PARTICLES = 100000;
	gNumParticles = NUM_PARTICLES;
	gParticleSystem = std::make_shared<ParticleSystem>(NUM_PARTICLES);

	//
	// emitter:
	//
	auto particleEmitter = std::make_shared<ParticleEmitter>();
	{		
		using namespace particleGenerators;
		particleEmitter->m_emitRate = (float)NUM_PARTICLES*0.395f;

		// pos:
		//auto posGenerator = std::make_shared<RoundPosGen>();
		gPosGenerator = std::make_shared<RoundPosGen>();
		gPosGenerator->m_center = Vec4d{ 0.0, 0.0, 0.0, 0.0 };
		gPosGenerator->m_radX = 0.15f;
		gPosGenerator->m_radY = 0.15f;
		//auto posGenerator = std::make_shared<particleGenerators::BoxPosGen>();
		//posGenerator->m_pos = Vec4d{ 0.0, 0.0, 0.0, 0.0 };
		//posGenerator->m_maxStartPosOffset = Vec4d{ 0.1, 0.1, 0.0, 0.0 };
		particleEmitter->addGenerator(gPosGenerator);
		
		gColGenerator = std::make_shared<BasicColorGen>();
		gColGenerator->m_minStartCol = Vec4d{ 0.7, 0.0, 0.7, 0.0 };
		gColGenerator->m_maxStartCol = Vec4d{ 1.0, 1.0, 1.0, 1.0 };
		gColGenerator->m_minEndCol = Vec4d{ 0.5, 0.0, 0.6, 0.0 };
		gColGenerator->m_maxEndCol = Vec4d{ 0.7, 0.5, 1.0, 0.0 };
		particleEmitter->addGenerator(gColGenerator);

		auto velGenerator = std::make_shared<BasicVelGen>();
		velGenerator->m_minStartVel = Vec4d{ 0.0f, 0.0f, 0.15f, 0.0f };
		velGenerator->m_maxStartVel = Vec4d{ 0.0f, 0.0f, 0.45f, 0.0f };
		particleEmitter->addGenerator(velGenerator);

		auto timeGenerator = std::make_shared<BasicTimeGen>();
		timeGenerator->m_minTime = 1.0;
		timeGenerator->m_maxTime = 3.5;	
		particleEmitter->addGenerator(timeGenerator);
	}
	gParticleSystem->addEmitter(particleEmitter);

	auto timeUpdater = std::make_shared<particleUpdaters::BasicTimeUpdater>();
	gParticleSystem->addUpdater(timeUpdater);

	auto colorUpdater = std::make_shared<particleUpdaters::BasicColorUpdater>();
	gParticleSystem->addUpdater(colorUpdater);

	auto eulerUpdater = std::make_shared<particleUpdaters::EulerUpdater>();
	eulerUpdater->m_globalAcceleration = Vec4d{ 0.0, 0.0, 0.0, 0.0 };
	gParticleSystem->addUpdater(eulerUpdater);

	//auto myUpdater = std::make_shared<MyUpdater>(0, NUM_PARTICLES);
	//gParticleSystem->addUpdater(myUpdater);

	gParticleRenderer = std::make_shared<GLParticleRenderer>();
	gParticleRenderer->generate(gParticleSystem.get(), false);

	gpuUpdate.init();
	gpuRender.init();

	ui::AddVar<int>("particles", &gNumParticles, "");
	ui::AddVar<int>("alive", &gNumAlive, "");
	ui::AddVar<double>("cpu particles", &cpuParticlesUpdate.m_time, "precision=3 group=timers");
	ui::AddVar<double>("cpu buffers", &cpuBuffersUpdate.m_time, "precision=3 group=timers");

	ui::AddVar<double>("gpu buffer", &gpuUpdate.getTime(), "precision=3 group=timers");
	ui::AddVar<double>("gpu render", &gpuRender.getTime(), "precision=3 group=timers");	

	ui::AddTweakColor4f("start col min", &gColGenerator->m_minStartCol.x, "");
	ui::AddTweakColor4f("start col max", &gColGenerator->m_maxStartCol.x, "");
	ui::AddTweakColor4f("end col min", &gColGenerator->m_minEndCol.x, "");
	ui::AddTweakColor4f("end col max", &gColGenerator->m_maxEndCol.x, "");

	ui::AddTweakDir3f("camera", &camera.cameraPosition.x, "");
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
void cleanUp()
{
	if (gParticleRenderer) gParticleRenderer->destroy();
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
}

///////////////////////////////////////////////////////////////////////////////
void pressSpecialKey(int key, int x, int y) 
{
	if (key == GLUT_KEY_UP)
		camera.cameraPosition[2] += 0.1f;
	else if (key == GLUT_KEY_DOWN)
		camera.cameraPosition[2] -= 0.1f;

	else if (key == GLUT_KEY_LEFT)
		camera.cameraPosition[0] += 0.1f;
	else if (key == GLUT_KEY_RIGHT)
		camera.cameraPosition[0] -= 0.1f;
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
	static double time = 0.0;
	time += deltaTime;

	gPosGenerator->m_center.x = 0.1*sinf((float)time*2.5);
	gPosGenerator->m_center.y = 0.1*cosf((float)time*2.5);
	gPosGenerator->m_radX = 0.15f + 0.05*sinf((float)time);
	gPosGenerator->m_radY = 0.15f + 0.05*sinf((float)time)*cosf((float)time*0.5);

	cpuParticlesUpdate.begin();
		gParticleSystem->update((FPType)deltaTime);
		gNumAlive = gParticleSystem->numAliveParticles();
	cpuParticlesUpdate.end();

	cpuBuffersUpdate.begin();
		gpuUpdate.begin();
			gParticleRenderer->update();
		gpuUpdate.end();		
	cpuBuffersUpdate.end();
	gpuUpdate.updateResults(TimerQuery::WaitOption::WaitForResults);
}

///////////////////////////////////////////////////////////////////////////////
void renderScene() 
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	//
	// camera
	//
	camera.modelviewMatrix = glm::lookAt(camera.cameraPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gParticleTexture);

	glEnable(GL_PROGRAM_POINT_SIZE);

	mProgram.use();
	mProgram.uniformMatrix4f("matProjection", glm::value_ptr(camera.projectionMatrix));
	mProgram.uniformMatrix4f("matModelview", glm::value_ptr(camera.modelviewMatrix));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	gpuRender.begin();
		gParticleRenderer->render();
	gpuRender.end();

	glDisable(GL_BLEND);

    mProgram.disable();

	gpuRender.updateResults(TimerQuery::WaitOption::WaitForResults);
}
