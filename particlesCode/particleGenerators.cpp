#include "particleGenerators.h"
#include <assert.h>
#include <algorithm>
#include <glm/common.hpp>
#include <glm/gtc/random.hpp>

namespace particleGenerators
{

	void BoxPosGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
	{
		Vec4d posMin{ m_pos.x - m_maxStartPosOffset.x, m_pos.y - m_maxStartPosOffset.y, m_pos.z - m_maxStartPosOffset.z, 1.0 };
		Vec4d posMax{ m_pos.x + m_maxStartPosOffset.x, m_pos.y + m_maxStartPosOffset.y, m_pos.z + m_maxStartPosOffset.z, 1.0 };

		for (size_t i = startId; i < endId; ++i)
		{
			p->m_pos[i] = glm::linearRand(posMin, posMax);
		}
	}

	void RoundPosGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
	{
		for (size_t i = startId; i < endId; ++i)
		{
			double ang = glm::linearRand(0.0, 3.141592*2.0);
			p->m_pos[i] = m_center + Vec4d(m_radX*sin(ang), m_radY*cos(ang), 0.0, 1.0);
		}
	}

	void BasicColorGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
	{
		for (size_t i = startId; i < endId; ++i)
		{
			p->m_startCol[i] = glm::linearRand(m_minStartCol, m_maxStartCol);
			p->m_endCol[i] = glm::linearRand(m_minEndCol, m_maxEndCol);
		}
	}

	void BasicVelGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
	{
		for (size_t i = startId; i < endId; ++i)
		{
			p->m_vel[i] = glm::linearRand(m_minStartVel, m_maxStartVel);
		}
	}

	void VelFromPosGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
	{
		for (size_t i = startId; i < endId; ++i)
		{
			FPType scale = static_cast<FPType>(glm::linearRand(m_minScale, m_maxScale));
			Vec4d vel = (p->m_pos[i] - m_offset);
			p->m_vel[i] = scale * vel;
		}
	}

	void BasicTimeGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
	{
		for (size_t i = startId; i < endId; ++i)
		{
			p->m_time[i].x = p->m_time[i].y = glm::linearRand(m_minTime, m_maxTime);
			p->m_time[i].z = (FPType)0.0;
			p->m_time[i].w = (FPType)1.0 / p->m_time[i].x;
		}
	}

}