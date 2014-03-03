#include "particleUpdaters.h"
#include <assert.h>
#include <algorithm>
#include <glm/common.hpp>
#include <glm/gtc/random.hpp>

namespace particles
{

	void EulerUpdater::update(double dt, ParticleData *p)
	{
		const glm::vec4 globalA{ dt * m_globalAcceleration.x, dt * m_globalAcceleration.y, dt * m_globalAcceleration.z, 0.0 };
		const float localDT = (float)dt;

		const unsigned int endId = p->m_countAlive;
		for (size_t i = 0; i < endId; ++i)
			p->m_acc[i] += globalA;

		for (size_t i = 0; i < endId; ++i)
			p->m_vel[i] += localDT * p->m_acc[i];

		for (size_t i = 0; i < endId; ++i)
			p->m_pos[i] += localDT * p->m_vel[i];
	}

	void BasicColorUpdater::update(double dt, ParticleData *p)
	{
		const unsigned int endId = p->m_countAlive;
		for (unsigned int i = 0; i < endId; ++i)
			p->m_col[i] = glm::mix(p->m_startCol[i], p->m_endCol[i], p->m_time[i].z);
	}

	void BasicTimeUpdater::update(double dt, ParticleData *p)
	{
		unsigned int endId = p->m_countAlive;
		const float localDT = (float)dt;
		for (unsigned int i = 0; i < endId; ++i)
		{
			p->m_time[i].x -= localDT;
			// interpolation: from 0 (start of life) till 1 (end of life)
			p->m_time[i].z = (float)1.0 - (p->m_time[i].x*p->m_time[i].w); // .w is 1.0/max life time		

			if (p->m_time[i].x < (float)0.0)
			{
				p->kill(i);
				endId = p->m_countAlive < p->m_count ? p->m_countAlive : p->m_count;
			}
		}
	}
}