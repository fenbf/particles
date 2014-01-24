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

#include "Particles.h"
#include "ParticleRenderer.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// globals

std::string Globals::ApplicationWindowName = "Draw Test Particles";

ShaderProgram mProgram;
ShaderProgram mProgramRef;
std::shared_ptr<ParticleSystem> gParticleSystem;
std::shared_ptr<GLParticleRenderer> gParticleRenderer;

unsigned int gParticleTexture = 0;

struct Camera
{
	glm::vec3 cameraPosition;
	glm::mat4 modelviewMatrix;
	glm::mat4 projectionMatrix;
} camera;

struct Geometry
{
	GLuint vertexBuffer, indexBuffer;
	GLuint vao;
	int numElements;
	GLuint textureMain;
	GLuint textureAdditional;
} geometry;

void createVertexBuffers()
{    
	float posData[] = {
		-0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f
	};

	glGenVertexArrays(1, &geometry.vao);
	glBindVertexArray(geometry.vao);

	glGenBuffers(1, &geometry.vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 9*sizeof(float), &posData[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3)*sizeof(float), (void *)((0)*sizeof(float)));

	//glGenBuffers(1, &geometry.indexBuffer);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry.indexBuffer);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, geometry.numElements*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

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

	if (!shaderLoader::loadAndBuildShaderPairFromFile(&mProgramRef, "shaders/simple.vs", "shaders/simple.fs"))
		return false;

	//
	// generate texture:
	//
	if (!textureLoader::loadTexture("data\\particle.png", &gParticleTexture))
		return false;

	mProgram.use();
	mProgram.uniform1i("tex", 0);
	mProgram.disable();

    createVertexBuffers();


	//
	// particles
	//
	const size_t NUM_PARTICLES = 1000;
	gParticleSystem = std::make_shared<ParticleSystem>(NUM_PARTICLES);

	auto particleEmitter = std::make_shared<BasicParticleEmitter>(0, NUM_PARTICLES);
	particleEmitter->m_emitRate = 1000.0;
	particleEmitter->m_pos = Vec4d{ 0.0 };
	particleEmitter->m_maxStartPosOffset = Vec4d{ 0.0 };
	particleEmitter->m_minStartCol = Vec4d{ 0.0 };
	particleEmitter->m_maxStartCol = Vec4d{ 1.0, 1.0, 1.0, 1.0 };
	particleEmitter->m_minEndCol = Vec4d{ 0.0 };
	particleEmitter->m_maxEndCol = Vec4d{ 1.0, 1.0, 1.0, 0.0 };
	particleEmitter->m_minTime = 0.0;
	particleEmitter->m_maxTime = 2.0;
	particleEmitter->m_minStartVel = Vec4d{ -0.1f, 0.1f, -0.1f, 0.0f };
	particleEmitter->m_maxStartVel = Vec4d{ 0.1f, 1.0f, 0.1f, 0.0f };
	gParticleSystem->addEmitter(particleEmitter);

	auto timeUpdater = std::make_shared<BasicTimeParticleUpdater>(0, NUM_PARTICLES);
	gParticleSystem->addUpdater(timeUpdater);

	auto colorUpdater = std::make_shared<BasicColorParticleUpdater>(0, NUM_PARTICLES);
	gParticleSystem->addUpdater(colorUpdater);

	auto eulerUpdater = std::make_shared<EulerParticleUpdater>(0, NUM_PARTICLES);
	eulerUpdater->m_globalAcceleration = Vec4d{ 0.0 };
	gParticleSystem->addUpdater(eulerUpdater);

	gParticleRenderer = std::make_shared<GLParticleRenderer>();
	gParticleRenderer->generate(gParticleSystem.get(), false);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
void cleanUp()
{
	if (gParticleRenderer) gParticleRenderer->destroy();
	glDeleteTextures(1, &gParticleTexture);

	glDeleteVertexArrays(1, &geometry.vao);
	glDeleteBuffers(1, &geometry.vertexBuffer);
	glDeleteBuffers(1, &geometry.indexBuffer);
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
		camera.cameraPosition[2] += 1;
	else if (key == GLUT_KEY_DOWN)
		camera.cameraPosition[2] -= 1;

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
	gParticleSystem->update((FPType)deltaTime);
	gParticleRenderer->update();
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


	//mProgramRef.use();
	//mProgramRef.uniformMatrix4f("matProjection", glm::value_ptr(camera.projectionMatrix));
	//mProgramRef.uniformMatrix4f("matModelview", glm::value_ptr(camera.modelviewMatrix));

	//glBindVertexArray(geometry.vao);
	////glBindBuffer(GL_ARRAY_BUFFER, geometry.vertexBuffer);
	////glDrawArrays(GL_TRIANGLES, 0, 3);
	//glBindVertexArray(0);

	//mProgramRef.disable();

	//
	// shader setup
	//
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gParticleTexture);
	mProgram.use();
	mProgram.uniformMatrix4f("modelviewMat", glm::value_ptr(camera.projectionMatrix));
	mProgram.uniformMatrix4f("projectionMat", glm::value_ptr(camera.modelviewMatrix));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	gParticleRenderer->render();

	glDisable(GL_BLEND);

    mProgram.disable();
}
