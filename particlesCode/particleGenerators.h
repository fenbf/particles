#pragma once

#include <vector>
#include "commonMath.h"
#include "particles.h"

namespace particles
{
	namespace generators
	{

		class BoxPosGen : public ParticleGenerator
		{
		public:
			glm::vec4 m_pos;
			glm::vec4 m_maxStartPosOffset;
		public:
			BoxPosGen() : m_pos(0.0), m_maxStartPosOffset(0.0) { }

			virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
		};

		class RoundPosGen : public ParticleGenerator
		{
		public:
			glm::vec4 m_center;
			float m_radX;
			float m_radY;
		public:
			RoundPosGen() : m_center(0.0), m_radX(0.0), m_radY(0.0) { }
			RoundPosGen(const glm::vec4 &center, double radX, double radY)
				: m_center(center)
				, m_radX((float)radX)
				, m_radY((float)radY)
			{ }

			virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
		};

		class BasicColorGen : public ParticleGenerator
		{
		public:
			glm::vec4 m_minStartCol;
			glm::vec4 m_maxStartCol;
			glm::vec4 m_minEndCol;
			glm::vec4 m_maxEndCol;
		public:
			BasicColorGen() : m_minStartCol(0.0), m_maxStartCol(0.0), m_minEndCol(0.0), m_maxEndCol(0.0) { }

			virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
		};

		class BasicVelGen : public ParticleGenerator
		{
		public:
			glm::vec4 m_minStartVel;
			glm::vec4 m_maxStartVel;
		public:
			BasicVelGen() : m_minStartVel(0.0), m_maxStartVel(0.0) { }

			virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
		};

		class SphereVelGen : public ParticleGenerator
		{
		public:
			float m_minVel;
			float m_maxVel;
		public:
			SphereVelGen() : m_minVel(0.0), m_maxVel(0.0) { }

			virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
		};

		class VelFromPosGen : public ParticleGenerator
		{
		public:
			glm::vec4 m_offset;
			float m_minScale;
			float m_maxScale;
		public:
			VelFromPosGen() : m_offset(0.0), m_minScale(0.0), m_maxScale(0.0) { }
			VelFromPosGen(const glm::vec4 &off, double minS, double maxS)
				: m_offset(off)
				, m_minScale((float)minS)
				, m_maxScale((float)maxS)
			{ }

			virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
		};

		class BasicTimeGen : public ParticleGenerator
		{
		public:
			float m_minTime;
			float m_maxTime;
		public:
			BasicTimeGen() : m_minTime(0.0), m_maxTime(0.0) { }

			virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
		};
	}
}