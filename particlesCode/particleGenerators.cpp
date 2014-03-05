#include "particleGenerators.h"
#include <assert.h>
#include <algorithm>
#include <glm/common.hpp>
#include <glm/gtc/random.hpp>

#ifndef M_PI
	#define M_PI 		3.1415926535897932384626433832795f
	#define M_2_PI 		6.28318530717958647692528676655901f		// PI*2
#endif

namespace particles
{
	namespace generators
	{
		void BoxPosGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			glm::vec4 posMin{ m_pos.x - m_maxStartPosOffset.x, m_pos.y - m_maxStartPosOffset.y, m_pos.z - m_maxStartPosOffset.z, 1.0 };
			glm::vec4 posMax{ m_pos.x + m_maxStartPosOffset.x, m_pos.y + m_maxStartPosOffset.y, m_pos.z + m_maxStartPosOffset.z, 1.0 };

			for (size_t i = startId; i < endId; ++i)
			{
				p->m_pos[i] = glm::linearRand(posMin, posMax);
			}
		}

		void RoundPosGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			for (size_t i = startId; i < endId; ++i)
			{
				double ang = glm::linearRand(0.0, M_PI*2.0);
				p->m_pos[i] = m_center + glm::vec4(m_radX*sin(ang), m_radY*cos(ang), 0.0, 1.0);
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

		void SphereVelGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			float phi, theta, v, r;
			for (size_t i = startId; i < endId; ++i)
			{
				phi = glm::linearRand(-M_PI, M_PI);
				theta = glm::linearRand(-M_PI, M_PI);
				v = glm::linearRand(m_minVel, m_maxVel);

				r = v*sinf(phi);
				p->m_vel[i].z = v*cosf(phi);
				p->m_vel[i].x = r*cosf(theta);
				p->m_vel[i].y = r*sinf(theta);
			}
		}

		void VelFromPosGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			for (size_t i = startId; i < endId; ++i)
			{
				float scale = static_cast<float>(glm::linearRand(m_minScale, m_maxScale));
				glm::vec4 vel = (p->m_pos[i] - m_offset);
				p->m_vel[i] = scale * vel;
			}
		}

		void BasicTimeGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			for (size_t i = startId; i < endId; ++i)
			{
				p->m_time[i].x = p->m_time[i].y = glm::linearRand(m_minTime, m_maxTime);
				p->m_time[i].z = (float)0.0;
				p->m_time[i].w = (float)1.0 / p->m_time[i].x;
			}
		}
	}
}