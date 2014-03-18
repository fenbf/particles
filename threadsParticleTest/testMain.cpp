#include <iostream>
#include <chrono>

#include "glm\glm.hpp"
#include "commonMath.h"
#include "particles.h"
#include "particleGenerators.h"
#include "particleUpdaters.h"

using namespace particles;

int main()
{
	size_t NUM_PARTICLES{ 10000 };
	const double DELTA_TIME{ 1.0 / 60.0 };	// 60 fps
	const int FRAME_COUNT{ 100 };

	ParticleSystem particleSystem{ NUM_PARTICLES };

	std::cout << ParticleSystem::computeMemoryUsage(particleSystem) / 1024 <<" kbytes" << std::endl;

	//
	// emitter:
	//
	auto particleEmitter = std::make_shared<particles::ParticleEmitter>();
	{
		particleEmitter->m_emitRate = (float)NUM_PARTICLES*0.45f;

		// pos:
		auto posGenerator = std::make_shared<particles::generators::RoundPosGen>();
		posGenerator->m_center = glm::vec4{ 0.0, 0.0, 0.0, 0.0 };
		posGenerator->m_radX = 0.15f;
		posGenerator->m_radY = 0.15f;
		particleEmitter->addGenerator(posGenerator);

		auto colGenerator = std::make_shared<particles::generators::BasicColorGen>();
		colGenerator->m_minStartCol = glm::vec4{ 0.7, 0.0, 0.7, 1.0 };
		colGenerator->m_maxStartCol = glm::vec4{ 1.0, 1.0, 1.0, 1.0 };
		colGenerator->m_minEndCol = glm::vec4{ 0.5, 0.0, 0.6, 0.0 };
		colGenerator->m_maxEndCol = glm::vec4{ 0.7, 0.5, 1.0, 0.0 };
		particleEmitter->addGenerator(colGenerator);

		auto velGenerator = std::make_shared<particles::generators::BasicVelGen>();
		velGenerator->m_minStartVel = glm::vec4{ 0.0f, 0.0f, 0.15f, 0.0f };
		velGenerator->m_maxStartVel = glm::vec4{ 0.0f, 0.0f, 0.45f, 0.0f };
		particleEmitter->addGenerator(velGenerator);

		auto timeGenerator = std::make_shared<particles::generators::BasicTimeGen>();
		timeGenerator->m_minTime = 1.0;
		timeGenerator->m_maxTime = 3.5;
		particleEmitter->addGenerator(timeGenerator);
	}
	particleSystem.addEmitter(particleEmitter);

	auto timeUpdater = std::make_shared<particles::updaters::BasicTimeUpdater>();
	particleSystem.addUpdater(timeUpdater);

	auto colorUpdater = std::make_shared<particles::updaters::BasicColorUpdater>();
	//colorUpdater->m_minPos = glm::vec4{ -1.0f };
	//colorUpdater->m_maxPos = glm::vec4{ 1.0f };
	particleSystem.addUpdater(colorUpdater);

	auto eulerUpdater = std::make_shared<particles::updaters::EulerUpdater>();
	eulerUpdater->m_globalAcceleration = glm::vec4{ 0.0, 0.0, 0.0, 0.0 };
	particleSystem.addUpdater(eulerUpdater);

	auto startTime = std::chrono::high_resolution_clock::now();
	for (int frame = 0; frame < FRAME_COUNT; ++frame)
	{
		particleSystem.update(DELTA_TIME);
	}
	auto endTime = std::chrono::high_resolution_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " miliseconds" << std::endl;

	return 0;
}