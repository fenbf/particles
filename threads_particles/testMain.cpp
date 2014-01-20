#include <iostream>

#include "commonMath.h"
#include "particles.h"

int main()
{
	ParticleSystem particleSystem{ 10000 };

	std::cout << ParticleSystem::computeMemoryUsage(particleSystem) / 1024 <<" kbytes" << std::endl;

	return 0;
}