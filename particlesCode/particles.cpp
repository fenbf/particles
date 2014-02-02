#include "particles.h"
#include <assert.h>
#include <algorithm>


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
			destination->m_startCol[id] = source->m_startCol[i];
			destination->m_endCol[id] = source->m_endCol[i];
			destination->m_vel[id] = source->m_vel[i];
			destination->m_acc[id] = source->m_acc[i];
			destination->m_time[id] = source->m_time[i];
			destination->m_alive[id] = true;
			id++;
		}
	}

	destination->m_countAlive = id;
}

size_t ParticleData::computeMemoryUsage(const ParticleData &p) 
{ 
	return p.m_count * (7 * sizeof(Vec4d)+sizeof(bool)) + sizeof(size_t)* 2; 
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

void ParticleSystem::update(double dt)
{
	for (auto & em : m_emitters)
	{
		em->update(dt, &m_particles);
	}

	for (size_t i = 0; i < m_count; ++i)
	{
		m_particles.m_acc[i] = glm::vec4(0.0f);
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