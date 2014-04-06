#include "particleUpdaters.h"
#include <assert.h>
#include <algorithm>
#include <glm/common.hpp>
#include <glm/gtc/random.hpp>
#include <xmmintrin.h>

namespace particles
{
	namespace updaters
	{
		void EulerUpdater::update(double dt, ParticleData *p)
		{
			const glm::simdVec4 globalA{ (float)dt * m_globalAcceleration.x, (float)dt * m_globalAcceleration.y, (float)dt * m_globalAcceleration.z, 0.0f };
			const float localDT = (float)dt;

			__m256 ga = _mm256_set_m128(globalA.Data, globalA.Data);
			__m256 *pa, *pb, pc;
			__m256 ldt = _mm256_set1_ps(localDT);

			const unsigned int endId = p->m_countAlive;
			size_t i;
			for (i = 0; i < endId; i+=2)
			{
				 /*p->m_acc[i] += globalA;
				 p->m_acc[i+1] += globalA;*/
				pa = (__m256*)(&p->m_acc[i].x);
				*pa = _mm256_add_ps(*pa, ga);
				/*pa = (__m256*)(&p->m_acc[i+2].x);
				*pa = _mm256_add_ps(*pa, ga);*/
			}
			for (; i < endId; i++)
			{
				p->m_acc[i] += globalA;
			}

			for (i = 0; i < endId; i += 2)
			{
				//p->m_vel[i] += localDT * p->m_acc[i];
				//p->m_vel[i+1] += localDT * p->m_acc[i+1];

				pa = (__m256*)(&p->m_vel[i].x);
				pb = (__m256*)(&p->m_acc[i].x);
				pc = _mm256_mul_ps(*pb, ldt);
				*pa = _mm256_add_ps(*pa, pc);

				/*pa = (__m256*)(&p->m_vel[i+2].x);
				pb = (__m256*)(&p->m_acc[i+2].x);
				pc = _mm256_mul_ps(*pb, ldt);
				*pa = _mm256_add_ps(*pa, pc);*/
			}
			for (; i < endId; i++)
			{
				p->m_vel[i] += localDT * p->m_acc[i];
			}

			for (size_t i = 0; i < endId; i += 2)
			{
				//p->m_pos[i] += localDT * p->m_vel[i];
				//p->m_pos[i+1] += localDT * p->m_vel[i+1];
				pa = (__m256*)(&p->m_pos[i].x);
				pb = (__m256*)(&p->m_vel[i].x);
				pc = _mm256_mul_ps(*pb, ldt);
				*pa = _mm256_add_ps(*pa, pc);

				/*pa = (__m256*)(&p->m_pos[i+2].x);
				pb = (__m256*)(&p->m_vel[i+2].x);
				pc = _mm256_mul_ps(*pb, ldt);
				*pa = _mm256_add_ps(*pa, pc);*/
			}
			for (; i < endId; i++)
			{
				p->m_pos[i] += localDT * p->m_vel[i];
			}
		}

		void FloorUpdater::update(double dt, ParticleData *p)
		{
			const float localDT = (float)dt;

			const size_t endId = p->m_countAlive;
			glm::simdVec4 force;
			float normalFactor, velFactor;
			for (size_t i = 0; i < endId; ++i)
			{
				if (p->m_pos[i].y < m_floorY)
				{
					force = p->m_acc[i];
					normalFactor = glm::dot(force, glm::simdVec4(0.0f, 1.0f, 0.0f, 0.0f));
					if (normalFactor < 0.0f)
						force -= glm::simdVec4(0.0f, 1.0f, 0.0f, 0.0f) * normalFactor;

					velFactor = glm::dot(p->m_vel[i], glm::simdVec4(0.0f, 1.0f, 0.0f, 0.0f));
					//if (velFactor < 0.0)
					p->m_vel[i] -= glm::simdVec4(0.0f, 1.0f, 0.0f, 0.0f) * (1.0f + m_bounceFactor) * velFactor;

					p->m_acc[i] = force;
				}
			}

		}

		void AttractorUpdater::update(double dt, ParticleData *p)
		{
			const float localDT = (float)dt;

			const size_t endId = p->m_countAlive;
			const size_t countAttractors = m_attractors.size();
			glm::simdVec4 off;
			float dist;
			size_t a;

			glm::simdVec4 attr[8];
			for (size_t i = 0; i < countAttractors; ++i)
				attr[i] = glm::simdVec4(m_attractors[i]);

			for (size_t i = 0; i < endId; ++i)
			{
				for (a = 0; a < countAttractors; ++a)
				{
					off = attr[a] - p->m_pos[i];
					dist = glm::dot(off, off);
					dist = m_attractors[a].w / dist;
					p->m_acc[i] += off * dist;
				}
			}
		}

		void BasicColorUpdater::update(double dt, ParticleData *p)
		{
			__m128 x, y;
			__m128 t;

			const size_t endId = p->m_countAlive;
			size_t i;
			for (i = 0; i < endId; i+=2)
			{
				t = _mm_set1_ps(p->m_time[i].z);
				x = _mm_sub_ps(p->m_endCol[i].Data, p->m_startCol[i].Data);
				y = _mm_mul_ps(t, x);
				p->m_col[i].Data = _mm_add_ps(p->m_startCol[i].Data, y);

				t = _mm_set1_ps(p->m_time[i+1].z);
				x = _mm_sub_ps(p->m_endCol[i+1].Data, p->m_startCol[i+1].Data);
				y = _mm_mul_ps(t, x);
				p->m_col[i+1].Data = _mm_add_ps(p->m_startCol[i+1].Data, y);
			}

			for (; i < endId; ++i)
			{
				p->m_col[i] = glm::mix(p->m_startCol[i], p->m_endCol[i], glm::simdVec4{ p->m_time[i].z });
			}
		}

		void PosColorUpdater::update(double dt, ParticleData *p)
		{
			const size_t endId = p->m_countAlive;
			float scaler, scaleg, scaleb;
			float diffr = m_maxPos.x - m_minPos.x;
			float diffg = m_maxPos.y - m_minPos.y;
			float diffb = m_maxPos.z - m_minPos.z;
			for (size_t i = 0; i < endId; ++i)
			{
				scaler = (p->m_pos[i].x - m_minPos.x) / diffr;
				scaleg = (p->m_pos[i].y - m_minPos.y) / diffg;
				scaleb = (p->m_pos[i].z - m_minPos.z) / diffb;
				p->m_col[i].x = scaler;// glm::mix(p->m_startCol[i].r, p->m_endCol[i].r, scaler);
				p->m_col[i].y = scaleg;// glm::mix(p->m_startCol[i].g, p->m_endCol[i].g, scaleg);
				p->m_col[i].z = scaleb;// glm::mix(p->m_startCol[i].b, p->m_endCol[i].b, scaleb);
				p->m_col[i].w = glm::mix(p->m_startCol[i].w, p->m_endCol[i].w, p->m_time[i].z);
			}
		}

		void VelColorUpdater::update(double dt, ParticleData *p)
		{
			const size_t endId = p->m_countAlive;
			float scaler, scaleg, scaleb;
			float diffr = m_maxVel.x - m_minVel.x;
			float diffg = m_maxVel.y - m_minVel.y;
			float diffb = m_maxVel.z - m_minVel.z;
			for (size_t i = 0; i < endId; ++i)
			{
				scaler = (p->m_vel[i].x - m_minVel.x) / diffr;
				scaleg = (p->m_vel[i].y - m_minVel.y) / diffg;
				scaleb = (p->m_vel[i].z - m_minVel.z) / diffb;
				p->m_col[i].x = scaler;// glm::mix(p->m_startCol[i].r, p->m_endCol[i].r, scaler);
				p->m_col[i].y = scaleg;// glm::mix(p->m_startCol[i].g, p->m_endCol[i].g, scaleg);
				p->m_col[i].z = scaleb;// glm::mix(p->m_startCol[i].b, p->m_endCol[i].b, scaleb);
				p->m_col[i].w = glm::mix(p->m_startCol[i].w, p->m_endCol[i].w, p->m_time[i].z);
			}
		}

		void BasicTimeUpdater::update(double dt, ParticleData *p)
		{
			unsigned int endId = p->m_countAlive;
			const float localDT = (float)dt;

			if (endId == 0) return;

			for (size_t i = 0; i < endId; i+=2)
			{
				p->m_time[i].x -= localDT;
				// interpolation: from 0 (start of life) till 1 (end of life)
				p->m_time[i].z = 1.0f - (p->m_time[i].x*p->m_time[i].w); // .w is 1.0/max life time	

				p->m_time[i+1].x -= localDT;
				p->m_time[i+1].z = 1.0f - (p->m_time[i+1].x*p->m_time[i+1].w); // .w is 1.0/max life time	
			}

			if (endId % 2 != 0)
			{
				p->m_time[endId-1].x -= localDT;
				p->m_time[endId - 1].z = 1.0f - (p->m_time[endId - 1].x*p->m_time[endId - 1].w); // .w is 1.0/max life time
			}

			for (size_t i = 0; i < endId; ++i)
			{	
				if (p->m_time[i].x < 0.0f)
				{
					p->kill(i);
					endId = p->m_countAlive < p->m_count ? p->m_countAlive : p->m_count;
				}
			}
		}
	}
}