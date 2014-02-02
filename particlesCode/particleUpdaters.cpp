#include "particleUpdaters.h"
#include <assert.h>
#include <algorithm>
#include <glm/common.hpp>
#include <glm/gtc/random.hpp>

void BasicParticleEmitter::update(double dt, ParticleData *p)
{
	const int maxNewParticles = (int)(dt*m_emitRate);
	int newParticles = 0;

	Vec4d posMin { m_pos.x - m_maxStartPosOffset.x, m_pos.y - m_maxStartPosOffset.y, m_pos.z - m_maxStartPosOffset.z, 1.0 };
	Vec4d posMax { m_pos.x + m_maxStartPosOffset.x, m_pos.y + m_maxStartPosOffset.y, m_pos.z + m_maxStartPosOffset.z, 1.0 };

	for (unsigned int i = m_idStart; i < m_idEnd; ++i)
	{
		if (!p->m_alive[i] && newParticles < maxNewParticles)
		{
			p->m_pos[i] = glm::linearRand(posMin, posMax);
			p->m_startCol[i] = glm::linearRand(m_minStartCol, m_maxStartCol);
			p->m_endCol[i] = glm::linearRand(m_minEndCol, m_maxEndCol);
			p->m_vel[i] = glm::linearRand(m_minStartVel, m_maxStartVel);

			p->m_time[i].x = p->m_time[i].y = glm::linearRand(m_minTime, m_maxTime);
			p->m_time[i].z = (FPType)0.0;
			p->m_time[i].w = (FPType)1.0 / p->m_time[i].x;

			p->m_alive[i] = true;

			++newParticles;
		}
	}
}

void EulerParticleUpdater::update(double dt, ParticleData *p)
{
	const Vec4d globalA { dt * m_globalAcceleration.x, dt * m_globalAcceleration.y, dt * m_globalAcceleration.z, 0.0 };
	const FPType localDT = (FPType)dt;

	for (size_t i = m_idStart; i < m_idEnd; ++i)
		p->m_acc[i] += globalA;

	for (size_t i = m_idStart; i < m_idEnd; ++i)
		p->m_vel[i] += localDT * p->m_acc[i];

	for (size_t i = m_idStart; i < m_idEnd; ++i)
		p->m_pos[i] += localDT * p->m_vel[i];
}

void BasicColorParticleUpdater::update(double dt, ParticleData *p)
{
	for (unsigned int i = m_idStart; i < m_idEnd; ++i)
		p->m_col[i] = glm::mix(p->m_startCol[i], p->m_endCol[i], p->m_time[i].z);
}

void BasicTimeParticleUpdater::update(double dt, ParticleData *p)
{
	const FPType localDT = (FPType)dt;
	for (unsigned int i = m_idStart; i < m_idEnd; ++i)
	{
		p->m_time[i].x -= localDT;
		// interpolation: from 0 (start of life) till 1 (end of life)
		p->m_time[i].z = (FPType)1.0 - (p->m_time[i].x*p->m_time[i].w); // .w is 1.0/max life time
		p->m_alive[i] = p->m_alive[i] && (p->m_time[i].x > (FPType)0.0);
	}
}