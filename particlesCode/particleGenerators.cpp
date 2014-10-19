#include "particleGenerators.h"
#include <assert.h>
#include <algorithm>
#include <glm/common.hpp>
#include <glm/gtc/random.hpp>

#ifndef M_PI
	#define M_PI 		3.1415926535897932384626433832795f
	#define M_2_PI 		6.28318530717958647692528676655901f		// PI*2
#endif

#define RESTRICT __restrict
//#define RESTRICT

#define NO_RAND 0
#define RAND_DEFAULT 1
#define RAND_FAST 2
#define RAND_MODE RAND_FAST

static int RandSeed = 1;

// see here as well: http://code4k.blogspot.com/2009/10/random-float-number-using-x86-asm-code.html

// http://www.musicdsp.org/showone.php?id=273
// fast rand float, using full 32bit precision
// takes about 12 seconds for 2 billion calls
float Fast_RandFloat()
{
	RandSeed *= 16807;
	return (float)RandSeed * 4.6566129e-010f;
}

// http://www.rgba.org/articles/sfrand/sfrand.htm
static unsigned int mirand = 1;

float sfrand(void)
{
	unsigned int a;

	mirand *= 16807;

	a = (mirand & 0x007fffff) | 0x40000000;

	return(*((float*)&a) - 3.0f);
}

inline float randFloat(float a, float b)
{
#if RAND_MODE == RAND_DEFAULT
	return glm::linearRand(a, b);
#elif RAND_MODE == NO_RAND	
	// to test the bottleneck:
	return a; 
#elif RAND_MODE == RAND_FAST
	// using fastest method?
	return (0.5f + 0.5f*sfrand())*(b - a) + a;
#endif
}

inline glm::simdVec4 randVec4(const glm::vec4&a, const glm::vec4 &b)
{
#if RAND_MODE == RAND_DEFAULT
	return glm::simdVec4(glm::linearRand(a, b));
#elif RAND_MODE == NO_RAND	
	// to test the bottleneck
	return glm::simdVec4(a);
#elif RAND_MODE == RAND_FAST

	// using fastest method?
	return  glm::simdVec4(randFloat(a.x, b.x), randFloat(a.y, b.y), randFloat(a.z, b.z), randFloat(a.w, b.w));
#endif
}

namespace particles
{
	namespace generators
	{
		void BoxPosGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			//glm::simdVec4 * RESTRICT pos = p->m_pos;

			const glm::vec4 posMin{ m_pos.x - m_maxStartPosOffset.x, m_pos.y - m_maxStartPosOffset.y, m_pos.z - m_maxStartPosOffset.z, 1.0 };
			const glm::vec4 posMax{ m_pos.x + m_maxStartPosOffset.x, m_pos.y + m_maxStartPosOffset.y, m_pos.z + m_maxStartPosOffset.z, 1.0 };

			for (size_t i = startId; i < endId; i+=2)
			{
                p->m_pos[i] = randVec4(posMin, posMax);
                p->m_pos[i + 1] = randVec4(posMin, posMax);
			}
			if (endId % 2 != 0)
			{
                p->m_pos[endId - 1] = randVec4(posMin, posMax);
			}
		}

		void RoundPosGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			//glm::simdVec4 * RESTRICT pos = p->m_pos;

			float ang;
			for (size_t i = startId; i < endId; i+=2)
			{
				ang = randFloat(0.0f, M_PI*2.0f);
                p->m_pos[i] = glm::simdVec4(m_center + glm::vec4(m_radX*sinf(ang), m_radY*cosf(ang), 0.0f, 1.0f));
				
				ang = randFloat(0.0f, M_PI*2.0f);
                p->m_pos[i + 1] = glm::simdVec4(m_center + glm::vec4(m_radX*sinf(ang), m_radY*cosf(ang), 0.0f, 1.0f));
			}
			if (endId % 2 != 0)
			{
				ang = randFloat(0.0f, M_PI*2.0f);
                p->m_pos[endId - 1] = glm::simdVec4(m_center + glm::vec4(m_radX*sinf(ang), m_radY*cosf(ang), 0.0f, 1.0f));
			}
		}

		void BasicColorGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			/*glm::simdVec4 * RESTRICT startCol = p->m_startCol;
			glm::simdVec4 * RESTRICT endCol = p->m_endCol;*/

			for (size_t i = startId; i < endId; i+=2)
			{
                p->m_startCol[i] = randVec4(m_minStartCol, m_maxStartCol);
                p->m_endCol[i] = randVec4(m_minEndCol, m_maxEndCol);

                p->m_startCol[i + 1] = randVec4(m_minStartCol, m_maxStartCol);
                p->m_endCol[i + 1] = randVec4(m_minEndCol, m_maxEndCol);
			}
			if (endId % 2 != 0)
			{
                p->m_startCol[endId - 1] = randVec4(m_minStartCol, m_maxStartCol);
                p->m_endCol[endId - 1] = randVec4(m_minEndCol, m_maxEndCol);
			}
		}

		void BasicVelGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			//glm::simdVec4 * RESTRICT vel = p->m_vel;

			for (size_t i = startId; i < endId; i+=2)
			{
                p->m_vel[i] = randVec4(m_minStartVel, m_maxStartVel);
                p->m_vel[i + 1] = randVec4(m_minStartVel, m_maxStartVel);
			}
			if (endId % 2 != 0)
			{
                p->m_vel[endId - 1] = randVec4(m_minStartVel, m_maxStartVel);
			}
		}

		void SphereVelGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			float phi, theta, v, r;
			for (size_t i = startId; i < endId; ++i)
			{
				phi = randFloat(-M_PI, M_PI);
				theta = randFloat(-M_PI, M_PI);
				v = randFloat(m_minVel, m_maxVel);

				r = v*sinf(phi);
				p->m_vel[i] = glm::simdVec4(v*cosf(phi), r*cosf(theta), r*sinf(theta), 1.0f);
			}
		}

		void VelFromPosGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			glm::simdVec4 scalev;
			for (size_t i = startId; i < endId; ++i)
			{
				float scale = static_cast<float>(randFloat(m_minScale, m_maxScale));
				scalev = glm::simdVec4(scale, scale, scale, scale);
				glm::simdVec4 vel = (p->m_pos[i] - glm::simdVec4(m_offset));
				p->m_vel[i] = glm::simdVec4(scalev * vel);
			}
		}

		void BasicTimeGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
		{
			//glm::simdVec4 * RESTRICT t = p->m_time;

			for (size_t i = startId; i < endId; i+=2)
			{
                p->m_time[i].x = p->m_time[i].y = randFloat(m_minTime, m_maxTime);
				p->m_time[i].z = (float)0.0;
				p->m_time[i].w = (float)1.0 / p->m_time[i].x;

				p->m_time[i + 1].x = p->m_time[i + 1].y = randFloat(m_minTime, m_maxTime);
				p->m_time[i+1].z = (float)0.0;
				p->m_time[i+1].w = (float)1.0 / p->m_time[i+1].x;
			}
			if (endId % 2 != 0)
			{
				p->m_time[endId - 1].x = p->m_time[endId - 1].y = randFloat(m_minTime, m_maxTime);
				p->m_time[endId - 1].z = (float)0.0;
				p->m_time[endId - 1].w = (float)1.0 / p->m_time[endId-1].x;
			}
		}
	}
}