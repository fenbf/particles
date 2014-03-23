#include <iostream>
#include <chrono>

#include "glm\glm.hpp"
#include "commonMath.h"

#include "particles.h"
#include "effect.h"

using namespace particles;

int main()
{
	size_t NUM_PARTICLES{ 10000 };
	const double DELTA_TIME{ 1.0 / 60.0 };	// 60 fps
	const int FRAME_COUNT{ 100 };

	auto effect = EffectFactory::create("tunnel");
	effect->initialize();
	//std::cout << ParticleSystem::computeMemoryUsage(particleSystem) / 1024 << " kbytes" << std::endl;	

	auto startTime = std::chrono::high_resolution_clock::now();
	for (int frame = 0; frame < FRAME_COUNT; ++frame)
	{
		effect->cpuUpdate(DELTA_TIME);
	}
	auto endTime = std::chrono::high_resolution_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " miliseconds" << std::endl;

	return 0;
}