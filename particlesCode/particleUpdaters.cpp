#include "particleUpdaters.h"
#include <assert.h>
#include <algorithm>
#include <glm/common.hpp>
#include <glm/gtc/random.hpp>
#include <xmmintrin.h>

#define SSE_MODE_NONE 0
#define SSE_MODE_SSE2 1
#define SSE_MODE_AVX 2
#define SSE_MODE SSE_MODE_NONE

#define RESTRICT __restrict
//#define RESTRICT

namespace particles
{
	namespace updaters
	{
		void EulerUpdater::update(double dt, ParticleData *p)
		{
			const glm::simdVec4 globalA{ (float)dt * m_globalAcceleration.x, (float)dt * m_globalAcceleration.y, (float)dt * m_globalAcceleration.z, 0.0f };
			const float localDT = (float)dt;
            const unsigned int endId = p->m_countAlive;

            glm::simdVec4 * RESTRICT acc = p->m_acc;
            glm::simdVec4 * RESTRICT vel = p->m_vel;
            glm::simdVec4 * RESTRICT pos = p->m_pos;

#if SSE_MODE == SSE_MODE_NONE
            for (size_t i = 0; i < endId; ++i)
                acc[i] += globalA;

            for (size_t i = 0; i < endId; ++i)
                vel[i] += localDT * acc[i];

            for (size_t i = 0; i < endId; ++i)
                pos[i] += localDT * vel[i];
#elif SSE_MODE == SSE_MODE_SSE2
            __m128 ga = globalA.Data;
            __m128 *pa, *pb, pc;
            __m128 ldt = _mm_set_ps1(localDT);
            
            size_t i;
            for (i = 0; i < endId; i++)
            {
                pa = (__m128*)(&acc[i].x);
                *pa = _mm_add_ps(*pa, ga);
            }

            for (i = 0; i < endId; i ++)
            {
                pa = (__m128*)(&vel[i].x);
                pb = (__m128*)(&acc[i].x);
                pc = _mm_mul_ps(*pb, ldt);
                *pa = _mm_add_ps(*pa, pc);
            }

            for (size_t i = 0; i < endId; i++)
            {
                pa = (__m128*)(&pos[i].x);
                pb = (__m128*)(&vel[i].x);
                pc = _mm_mul_ps(*pb, ldt);
                *pa = _mm_add_ps(*pa, pc);
            }
#elif SSE_MODE == SSE_MODE_AVX
			__m256 ga = _mm256_set_m128(globalA.Data, globalA.Data);
			__m256 *pa, *pb, pc;
			__m256 ldt = _mm256_set1_ps(localDT);
			size_t i;
			for (i = 0; i < endId; i+=2)
			{
				pa = (__m256*)(&acc[i].x);
				*pa = _mm256_add_ps(*pa, ga);
			}
			for (; i < endId; i++)
			{
				acc[i] += globalA;
			}

			for (i = 0; i < endId; i += 2)
			{
				pa = (__m256*)(&vel[i].x);
				pb = (__m256*)(&acc[i].x);
				pc = _mm256_mul_ps(*pb, ldt);
				*pa = _mm256_add_ps(*pa, pc);
			}
			for (; i < endId; i++)
			{
				vel[i] += localDT * acc[i];
			}

			for (size_t i = 0; i < endId; i += 2)
			{
				pa = (__m256*)(&pos[i].x);
				pb = (__m256*)(&vel[i].x);
				pc = _mm256_mul_ps(*pb, ldt);
				*pa = _mm256_add_ps(*pa, pc);
			}
			for (; i < endId; i++)
			{
				pos[i] += localDT * vel[i];
			}
#endif // AVX
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
					//if (normalFactor < 0.0f)
						force -= glm::simdVec4(0.0f, 1.0f, 0.0f, 0.0f) * normalFactor;

					velFactor = glm::dot(p->m_vel[i], glm::simdVec4(0.0f, 1.0f, 0.0f, 0.0f));
					//if (velFactor < 0.0)
					p->m_vel[i] -= glm::simdVec4(0.0f, 1.0f, 0.0f, 0.0f) * (1.0f + m_bounceFactor) * velFactor;

					p->m_acc[i] = force;
				}
			}

		}

		inline float inverse(float x)
		{
			// re-interpret as a 32 bit integer
			unsigned int *i = (unsigned int*)&x;

			// adjust exponent
			//*i = 0x7F000000 - *i;
			*i = 0x7EEEEEEE - *i;
			return x;
		}

		inline float inverse2(float f)
		{
			int x = *reinterpret_cast<int *>(&f);
			x = 0x7EF311C2 - x;
			float inv = *reinterpret_cast<float *>(&x);
			//inv = inv * (2 - inv * f);
			return inv;
		}

		void AttractorUpdater::update(double dt, ParticleData *p)
		{
			const size_t countAttractors = m_attractors.size();
			glm::simdVec4 attr[8];
			for (size_t i = 0; i < countAttractors; ++i)
				attr[i] = glm::simdVec4(m_attractors[i]);

			const float localDT = (float)dt;

			glm::simdVec4 * __restrict acc = p->m_acc;
			glm::simdVec4 * __restrict vel = p->m_vel;
			glm::simdVec4 * __restrict pos = p->m_pos;

			const size_t endId = p->m_countAlive;
			glm::simdVec4 off = glm::simdVec4(0.0f);
#if SSE_MODE == SSE_MODE_NONE
			float dist;
#elif SSE_MODE == SSE_MODE_SSE2 || SSE_MODE == SSE_MODE_AVX
			__m128 tempDist;
#endif
			
			size_t a;
			for (size_t i = 0; i < endId; ++i)
			{
				for (a = 0; a < countAttractors; ++a)
				{
#if SSE_MODE == SSE_MODE_NONE
					off = attr[a] - p->m_pos[i];
					dist = off.x*off.x + off.y*off.y + off.z*off.z;
					dist = attr[a].w / dist;// *inverse(dist);
					p->m_acc[i] += off * dist;
#elif SSE_MODE == SSE_MODE_SSE2 || SSE_MODE == SSE_MODE_AVX
					off = attr[a] - p->m_pos[i];
					tempDist = _mm_dp_ps(off.Data, off.Data, 0x71);
					tempDist.m128_f32[0] = attr[a].w / tempDist.m128_f32[0];// *inverse2(fabs(tempDist.m128_f32[0]);
					p->m_acc[i] += off * tempDist.m128_f32[0];
#endif
				}
			}
		}

		void BasicColorUpdater::update(double dt, ParticleData *p)
		{
			const size_t endId = p->m_countAlive;

            glm::simdVec4 * RESTRICT col = p->m_col;
            glm::simdVec4 * RESTRICT startCol = p->m_startCol;
            glm::simdVec4 * RESTRICT endCol = p->m_endCol;
            glm::simdVec4 * RESTRICT ti = p->m_time;

#if SSE_MODE == SSE_MODE_NONE
			for (size_t i = 0; i < endId; ++i)
				col[i] = glm::mix(startCol[i], endCol[i], glm::simdVec4(ti[i].z));
#elif SSE_MODE == SSE_MODE_SSE2 || SSE_MODE == SSE_MODE_AVX
			__m128 x, y;
			__m128 t;

			for (size_t i = 0; i < endId; i ++)
			{
				t = _mm_set1_ps(ti[i].z);
				x = _mm_sub_ps(endCol[i].Data, startCol[i].Data);
				y = _mm_mul_ps(t, x);
				col[i].Data = _mm_add_ps(startCol[i].Data, y);
			}
//#elif SSE_MODE == SSE_MODE_AVX
//			__m256 x, y, cs, ce, z;
//			__m256 t;
//
//			size_t i;
//			for (i = 0; i < endId; i += 2)
//			{
//				t = _mm256_set_m128(_mm_set1_ps(p->m_time[i].z), _mm_set1_ps(p->m_time[i+1].z));
//				cs = _mm256_loadu2_m128(&p->m_startCol[i].x, &p->m_startCol[i + 1].x);
//				ce = _mm256_loadu2_m128(&p->m_endCol[i].x, &p->m_endCol[i + 1].x);
//				x = _mm256_sub_ps(ce, cs);
//				y = _mm256_mul_ps(t, x);
//				z = _mm256_add_ps(cs, y);
//				p->m_col[i].Data = _mm_load_ps(z.m256_f32);
//				p->m_col[i + 1].Data = _mm_load_ps(z.m256_f32+4);
//			}
//
//			for (; i < endId; ++i)
//			{
//				p->m_col[i] = glm::mix(p->m_startCol[i], p->m_endCol[i], glm::simdVec4{ p->m_time[i].z });
//			}
#endif			
		}

		void PosColorUpdater::update(double dt, ParticleData *p)
		{
            glm::simdVec4 * RESTRICT col = p->m_col;
            glm::simdVec4 * RESTRICT pos = p->m_pos;
            glm::simdVec4 * RESTRICT startCol = p->m_startCol;
            glm::simdVec4 * RESTRICT endCol = p->m_endCol;
            glm::simdVec4 * RESTRICT t = p->m_time;

			const size_t endId = p->m_countAlive;
			float scaler, scaleg, scaleb;
			float diffr = m_maxPos.x - m_minPos.x;
			float diffg = m_maxPos.y - m_minPos.y;
			float diffb = m_maxPos.z - m_minPos.z;
			for (size_t i = 0; i < endId; ++i)
			{
				scaler = (pos[i].x - m_minPos.x) / diffr;
				scaleg = (pos[i].y - m_minPos.y) / diffg;
				scaleb = (pos[i].z - m_minPos.z) / diffb;
				col[i].x = scaler;// glm::mix(p->m_startCol[i].r, p->m_endCol[i].r, scaler);
				col[i].y = scaleg;// glm::mix(p->m_startCol[i].g, p->m_endCol[i].g, scaleg);
				col[i].z = scaleb;// glm::mix(p->m_startCol[i].b, p->m_endCol[i].b, scaleb);
				col[i].w = glm::mix(startCol[i].w, endCol[i].w, t[i].z);
			}
		}

		void VelColorUpdater::update(double dt, ParticleData *p)
		{
			const size_t endId = p->m_countAlive;

			// no sse version, did not performed better

            glm::simdVec4 * RESTRICT col = p->m_col;
            glm::simdVec4 * RESTRICT vel = p->m_vel;
            glm::simdVec4 * RESTRICT startCol = p->m_startCol;
            glm::simdVec4 * RESTRICT endCol = p->m_endCol;
            glm::simdVec4 * RESTRICT t = p->m_time;

			float scaler, scaleg, scaleb;
			float diffr = m_maxVel.x - m_minVel.x;
			float diffg = m_maxVel.y - m_minVel.y;
			float diffb = m_maxVel.z - m_minVel.z;
			for (size_t i = 0; i < endId; ++i)
			{
				scaler = (vel[i].x - m_minVel.x) / diffr;
				scaleg = (vel[i].y - m_minVel.y) / diffg;
				scaleb = (vel[i].z - m_minVel.z) / diffb;
				col[i].x = scaler;// glm::mix(p->m_startCol[i].r, p->m_endCol[i].r, scaler);
				col[i].y = scaleg;// glm::mix(p->m_startCol[i].g, p->m_endCol[i].g, scaleg);
				col[i].z = scaleb;// glm::mix(p->m_startCol[i].b, p->m_endCol[i].b, scaleb);
				col[i].w = glm::mix(startCol[i].w, endCol[i].w, t[i].z);
			}
		}

		void BasicTimeUpdater::update(double dt, ParticleData *p)
		{
			unsigned int endId = p->m_countAlive;
			const float localDT = (float)dt;

			if (endId == 0) return;

            glm::simdVec4 * RESTRICT t = p->m_time;

			for (size_t i = 0; i < endId; i+=2)
			{
				t[i].x -= localDT;
				// interpolation: from 0 (start of life) till 1 (end of life)
				t[i].z = 1.0f - (t[i].x*t[i].w); // .w is 1.0/max life time	

				t[i+1].x -= localDT;
				t[i+1].z = 1.0f - (t[i+1].x*t[i+1].w); // .w is 1.0/max life time	
			}

			if (endId % 2 != 0)
			{
				t[endId-1].x -= localDT;
				t[endId - 1].z = 1.0f - (t[endId - 1].x*t[endId - 1].w); // .w is 1.0/max life time
			}

			for (size_t i = 0; i < endId; ++i)
			{	
				if (t[i].x < 0.0f)
				{
					p->kill(i);
					endId = p->m_countAlive < p->m_count ? p->m_countAlive : p->m_count;
				}
			}
		}
	}
}