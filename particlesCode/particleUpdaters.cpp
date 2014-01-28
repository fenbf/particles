#include "particleUpdaters.h"
#include <assert.h>
#include <algorithm>

inline FPType interpolateInRange(FPType a, FPType b, float p)
{
	return a + p * (b - a);
}

inline FPType randNormal(FPType a, FPType b)
{
	FPType u1 = (FPType)rand() / RAND_MAX;
	FPType u2 = (FPType)rand() / RAND_MAX;
	FPType r = (FPType)sqrt(-(FPType)2.0*(FPType)log(u1))*(FPType)cos((FPType)2.0*(FPType)3.141592*u2);
	return a + (b - a) * (r*(FPType)0.25 + (FPType)0.25);
}

inline FPType randLinear(FPType a, FPType b)
{
	FPType u1 = (FPType)rand() / RAND_MAX;
	return a + (b - a) * u1;
}

inline FPType randNormalOffset(FPType a, FPType b)
{
	return (FPType)a + randNormal(0.0, b);
}

void BasicParticleEmitter::update(FPType dt, ParticleData *p)
{
	const int maxNewParticles = (int)(dt*m_emitRate);
	int newParticles = 0;

	Vec4d posMin { m_pos.x - m_maxStartPosOffset.x, m_pos.y - m_maxStartPosOffset.y, m_pos.z - m_maxStartPosOffset.z };
	Vec4d posMax { m_pos.x + m_maxStartPosOffset.x, m_pos.y + m_maxStartPosOffset.y, m_pos.z + m_maxStartPosOffset.z };

	for (unsigned int i = m_idStart; i < m_idEnd; ++i)
	{
		if (!p->m_alive[i] && newParticles < maxNewParticles)
		{
			p->m_pos[i].x = randLinear(posMin.x, posMax.x);
			p->m_pos[i].y = randLinear(posMin.y, posMax.y);
			p->m_pos[i].z = randLinear(posMin.z, posMax.z);
			p->m_pos[i].w = (FPType)1.0;

			p->m_startCol[i].r = randLinear(m_minStartCol.r, m_maxStartCol.r);
			p->m_startCol[i].g = randLinear(m_minStartCol.g, m_maxStartCol.g);
			p->m_startCol[i].b = randLinear(m_minStartCol.b, m_maxStartCol.b);
			p->m_startCol[i].a = randLinear(m_minStartCol.a, m_maxStartCol.a);

			p->m_endCol[i].r = randLinear(m_minEndCol.r, m_maxEndCol.r);
			p->m_endCol[i].g = randLinear(m_minEndCol.g, m_maxEndCol.g);
			p->m_endCol[i].b = randLinear(m_minEndCol.b, m_maxEndCol.b);
			p->m_endCol[i].a = randLinear(m_minEndCol.a, m_maxEndCol.a);

			p->m_vel[i].x = randLinear(m_minStartVel.x, m_maxStartVel.x);
			p->m_vel[i].y = randLinear(m_minStartVel.y, m_maxStartVel.y);
			p->m_vel[i].z = randLinear(m_minStartVel.z, m_maxStartVel.z);
			p->m_vel[i].w = (FPType)0.0;

			p->m_time[i].x = p->m_time[i].y = randLinear(m_minTime, m_maxTime);
			p->m_time[i].z = (FPType)0.0;
			p->m_time[i].w = (FPType)1.0 / p->m_time[i].x;

			p->m_alive[i] = true;

			++newParticles;
		}
	}
}

void EulerParticleUpdater::update(FPType dt, ParticleData *p)
{
	const Vec4d globalA{ dt * m_globalAcceleration.x, dt * m_globalAcceleration.y, dt * m_globalAcceleration.z };

	for (size_t i = m_idStart; i < m_idEnd; ++i)
		p->m_acc[i].add3(globalA);

	for (size_t i = m_idStart; i < m_idEnd; ++i)
	{
		p->m_vel[i].x += dt * p->m_acc[i].x;
		p->m_vel[i].y += dt * p->m_acc[i].y;
		p->m_vel[i].z += dt * p->m_acc[i].z;
	}

	for (size_t i = m_idStart; i < m_idEnd; ++i)
	{
		p->m_pos[i].x += p->m_vel[i].x*dt;
		p->m_pos[i].y += p->m_vel[i].y*dt;
		p->m_pos[i].z += p->m_vel[i].z*dt;
	}
}