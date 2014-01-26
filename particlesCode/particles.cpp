#include "particles.h"
#include <assert.h>
#include <algorithm>

//extern TVec4<FPType> Vec4d;

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

////////////////////////////////////////////////////////////////////////////////
// ParticleData class 

void ParticleData::generate(size_t maxSize)
{
	m_count = maxSize;
	m_countAlive = 0;

	m_pos.resize(m_count);
	m_col.resize(m_count);
	m_startCol.resize(m_count);
	m_endCol.resize(m_count);
	m_vel.resize(m_count);
	m_acc.resize(m_count);
	m_time.resize(m_count);
	m_alive.resize(m_count);
}

void ParticleData::copyOnlyAlive(const ParticleData *source, ParticleData *destination)
{
	assert(source->m_count == destination->m_count);

	size_t id = 0;
	for (size_t i = 0; i < source->m_count; ++i)
	{
		if (source->m_alive[i])
		{
			destination->m_pos[id] = source->m_pos[i];
			destination->m_col[id] = source->m_col[i];
			/*destination->m_startCol[id] = source->m_startCol[i];
			destination->m_endCol[id] = source->m_endCol[i];
			destination->m_vel[id] = source->m_vel[i];
			destination->m_acc[id] = source->m_acc[i];
			destination->m_time[id] = source->m_time[i];
			destination->m_alive[id] = true;*/
			id++;
		}
	}

	destination->m_countAlive = id;
}

size_t ParticleData::computeMemoryUsage(const ParticleData &p) 
{ 
	return p.m_count * (7 * sizeof(Vec4d)+sizeof(bool)) + sizeof(size_t)* 2; 
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

////////////////////////////////////////////////////////////////////////////////
// ParticleSystem class 

////////////////////////////////////////////////////////////////////////////////
ParticleSystem::ParticleSystem(size_t maxCount)
{
	m_count = maxCount;
	m_particles.generate(maxCount);
	m_aliveParticles.generate(maxCount);

	for (auto &alive : m_particles.m_alive)
		alive = false;
}

void ParticleSystem::update(FPType dt)
{
	for (auto & em : m_emitters)
	{
		em->update(dt, &m_particles);
	}

	for (size_t i = 0; i < m_count; ++i)
	{
		m_particles.m_acc[i].setAll(0.0);
	}

	for (auto & up : m_updaters)
	{
		up->update(dt, &m_particles);
	}

	ParticleData::copyOnlyAlive(&m_particles, &m_aliveParticles);
}

size_t ParticleSystem::computeMemoryUsage(const ParticleSystem &p)
{
	return 2 * ParticleData::computeMemoryUsage(p.m_particles);
}