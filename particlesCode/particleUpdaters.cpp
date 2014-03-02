#include "particleUpdaters.h"
#include <assert.h>
#include <algorithm>
#include <glm/common.hpp>
#include <glm/gtc/random.hpp>

void BasicParticleEmitter::generatePos(double dt, ParticleData *p, size_t startId, size_t endId)
{
	Vec4d posMin { m_pos.x - m_maxStartPosOffset.x, m_pos.y - m_maxStartPosOffset.y, m_pos.z - m_maxStartPosOffset.z, 1.0 };
	Vec4d posMax { m_pos.x + m_maxStartPosOffset.x, m_pos.y + m_maxStartPosOffset.y, m_pos.z + m_maxStartPosOffset.z, 1.0 };

	for (unsigned int i = startId; i < endId; ++i)
	{
		p->m_pos[i] = glm::linearRand(posMin, posMax);
	}
}

void BasicParticleEmitter::generateCol(double dt, ParticleData *p, size_t startId, size_t endId)
{
	for (unsigned int i = startId; i < endId; ++i)
	{
		p->m_startCol[i] = glm::linearRand(m_minStartCol, m_maxStartCol);
		p->m_endCol[i] = glm::linearRand(m_minEndCol, m_maxEndCol);
	}
}

void BasicParticleEmitter::generateOther(double dt, ParticleData *p, size_t startId, size_t endId)
{
	for (unsigned int i = startId; i < endId; ++i)
	{
		p->m_vel[i] = glm::linearRand(m_minStartVel, m_maxStartVel);
	}
}

void BasicParticleEmitter::generateTime(double dt, ParticleData *p, size_t startId, size_t endId)
{
	for (unsigned int i = startId; i < endId; ++i)
	{
		p->m_time[i].x = p->m_time[i].y = glm::linearRand(m_minTime, m_maxTime);
		p->m_time[i].z = (FPType)0.0;
		p->m_time[i].w = (FPType)1.0 / p->m_time[i].x;
	}
}

void EulerParticleUpdater::update(double dt, ParticleData *p)
{
	const Vec4d globalA { dt * m_globalAcceleration.x, dt * m_globalAcceleration.y, dt * m_globalAcceleration.z, 0.0 };
	const FPType localDT = (FPType)dt;

	const unsigned int endId = p->m_countAlive;
	for (size_t i = 0; i < endId; ++i)
		p->m_acc[i] += globalA;

	for (size_t i = 0; i < endId; ++i)
		p->m_vel[i] += localDT * p->m_acc[i];

	for (size_t i = 0; i < endId; ++i)
		p->m_pos[i] += localDT * p->m_vel[i];
}

void BasicColorParticleUpdater::update(double dt, ParticleData *p)
{
	const unsigned int endId = p->m_countAlive;
	for (unsigned int i = 0; i < endId; ++i)
		p->m_col[i] = glm::mix(p->m_startCol[i], p->m_endCol[i], p->m_time[i].z);
}

void BasicTimeParticleUpdater::update(double dt, ParticleData *p)
{
	unsigned int endId = p->m_countAlive;
	const FPType localDT = (FPType)dt;
	for (unsigned int i = 0; i < endId; ++i)
	{
		p->m_time[i].x -= localDT;
		// interpolation: from 0 (start of life) till 1 (end of life)
		p->m_time[i].z = (FPType)1.0 - (p->m_time[i].x*p->m_time[i].w); // .w is 1.0/max life time		

		if (p->m_time[i].x < (FPType)0.0)
		{
			p->kill(i);
			endId = p->m_countAlive < p->m_count ? p->m_countAlive : p->m_count;
		}
	}
}