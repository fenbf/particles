#include <iostream>
#include <map>
#include <Windows.h>

#include "glm\glm.hpp"
#include "commonMath.h"

#include "particles.h"
#include "effect.h"

using namespace particles;

int main()
{
	const std::vector<std::string> EFFECTS_NAME{ "tunnel" , "attractors", "fountain" };
	const size_t START_NUM_PARTICLES{ 1000 };
	const size_t END_NUM_PARTICLES{ 301000 };
	const size_t PARTICLES_NUM_STEPS{ 30 };	
	const size_t NUM_PARTICLES_STEP{ (END_NUM_PARTICLES - START_NUM_PARTICLES) / PARTICLES_NUM_STEPS };
	
	const double DELTA_TIME{ 1.0 / 60.0 };	// 60 fps
	const size_t FRAME_COUNT{ 200 };

	// run the system so that it has some decent num of alive particles...

	std::cout << "count\t";
	for (const auto &n : EFFECTS_NAME)
		std::cout << n.c_str() << "\t";
	std::cout << std::endl;

	std::map<std::string, double> aliveToAllRatios;
	for (const auto &n : EFFECTS_NAME)
		aliveToAllRatios[n] = 0.0;

	for (size_t step = 0; step < PARTICLES_NUM_STEPS; ++step)
	{
		size_t numParticles{ START_NUM_PARTICLES + step*NUM_PARTICLES_STEP };
		std::cout << numParticles << "\t";

		for (const auto &n : EFFECTS_NAME)
		{
			auto e = EffectFactory::create(n.c_str());
			e->initialize(numParticles);

			LARGE_INTEGER freq, startTime, endTime, elapsedMicroseconds;
			QueryPerformanceFrequency(&freq);
			QueryPerformanceCounter(&startTime);

			for (size_t frame = 0; frame < FRAME_COUNT; ++frame)
			{
				e->cpuUpdate(DELTA_TIME);
			}
			QueryPerformanceCounter(&endTime);
			elapsedMicroseconds.QuadPart = endTime.QuadPart - startTime.QuadPart;
			elapsedMicroseconds.QuadPart *= 100000;
			elapsedMicroseconds.QuadPart /= freq.QuadPart;

			std::cout << elapsedMicroseconds.QuadPart << "\t";

			aliveToAllRatios[n] += e->aliveToAllRatio();
		}
		std::cout << std::endl;
	}	
	for (const auto &n : EFFECTS_NAME)
		std::cout << aliveToAllRatios[n] / PARTICLES_NUM_STEPS*100.0 << '\t';
	std::cout << std::endl;

	return 0;
}