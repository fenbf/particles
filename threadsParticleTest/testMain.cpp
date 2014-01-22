#include <iostream>
#include <chrono>

#include "commonMath.h"
#include "particles.h"

int main()
{
	size_t NUM_PARTICLES{ 10000 };
	const double DELTA_TIME{ 1.0 / 60.0 };	// 60 fps
	const int FRAME_COUNT{ 100 };

	ParticleSystem particleSystem{ NUM_PARTICLES };

	std::cout << ParticleSystem::computeMemoryUsage(particleSystem) / 1024 <<" kbytes" << std::endl;

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
	particleSystem.addEmitter(particleEmitter);

	auto timeUpdater = std::make_shared<BasicTimeParticleUpdater>(0, NUM_PARTICLES);
	particleSystem.addEmitter(timeUpdater);

	auto colorUpdater = std::make_shared<BasicColorParticleUpdater>(0, NUM_PARTICLES);
	particleSystem.addEmitter(colorUpdater);

	auto eulerUpdater = std::make_shared<EulerParticleUpdater>(0, NUM_PARTICLES);
	particleSystem.addEmitter(eulerUpdater);

	auto startTime = std::chrono::high_resolution_clock::now();
	for (int frame = 0; frame < FRAME_COUNT; ++frame)
	{
		particleSystem.update((FPType)DELTA_TIME);
	}
	auto endTime = std::chrono::high_resolution_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " miliseconds" << std::endl;

	return 0;
}