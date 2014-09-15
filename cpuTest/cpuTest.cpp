#include <iostream>
#include <Windows.h>

#include "glm\glm.hpp"
#include "commonMath.h"

#include "particles.h"
#include "effect.h"

#include <chrono>

using namespace particles;

class CpuTimeQuery
{
protected:
	double m_time; 

protected:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_cpuTimePointStart;

public:
	void begin()
	{
		m_cpuTimePointStart = std::chrono::high_resolution_clock::now();
	}

	void end()
	{
		auto diff = std::chrono::high_resolution_clock::now() - m_cpuTimePointStart;
		auto mili = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
		m_time = 0.5*(m_time + (double)mili);
	}

	double timeInMilisec() const { return m_time;  }
	double timeInSeconds() const { return m_time*0.001; }
};

int main()
{
	const std::vector<std::string> EFFECTS_NAME { "tunnel", "attractors", "fountain" };
	const size_t START_NUM_PARTICLES{ 1000 };
	const size_t END_NUM_PARTICLES{ 301000 };
	const size_t PARTICLES_NUM_STEPS{ 30 };	
	const size_t NUM_PARTICLES_STEP{ (END_NUM_PARTICLES - START_NUM_PARTICLES) / PARTICLES_NUM_STEPS };
	
	const double DELTA_TIME{ 1.0 / 60.0 };	// 60 fps
	const size_t FRAME_COUNT{ 200 };

	CpuTimeQuery timer;

	std::cout << "count\t";
	for (const auto &n : EFFECTS_NAME)
		std::cout << n.c_str() << "|\t";
	std::cout << std::endl;
	std::cout << "-------|----------" << std::endl;

	std::cout.setf(std::ios::fixed, std::ios::floatfield);
	std::cout.precision(3);
	std::wcout << std::fixed;

	for (size_t step = 0; step < PARTICLES_NUM_STEPS; ++step)
	{
		size_t numParticles{ START_NUM_PARTICLES + step*NUM_PARTICLES_STEP };
		
		if (numParticles < 150000 || numParticles > 200000)
			continue;
		
		std::cout << numParticles << " | ";		

		for (const auto &n : EFFECTS_NAME)
		{
			auto e = EffectFactory::create(n.c_str());
			e->initialize(numParticles);

			timer.begin();

			for (size_t frame = 0; frame < FRAME_COUNT; ++frame)
			{
				e->cpuUpdate(DELTA_TIME);
			}
			timer.end();

			std::cout << timer.timeInMilisec() << " | ";
		}
		std::cout << std::endl;
	}	

	std::cout << "time in miliseconds" << std::endl;

	return 0;
}