#pragma once

#include <vector>
#include "commonMath.h"
#include "particles.h"

namespace particleGenerators
{

	class BoxPosGen : public ParticleGenerator
	{
	public:
		Vec4d m_pos{ 0.0 };
		Vec4d m_maxStartPosOffset{ 0.0 };
	public:
		BoxPosGen() { }

		virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
	};

	class RoundPosGen : public ParticleGenerator
	{
	public:
		Vec4d m_center{ 0.0 };
		FPType m_radX{ 0.0 };
		FPType m_radY{ 0.0 };
	public:
		RoundPosGen() { }
		RoundPosGen(const Vec4d &center, double radX, double radY)
			: m_center(center)
			, m_radX((FPType)radX)
			, m_radY((FPType)radY)
		{ }

		virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
	};

	class BasicColorGen : public ParticleGenerator
	{
	public:
		Vec4d m_minStartCol{ 0.0 };
		Vec4d m_maxStartCol{ 0.0 };
		Vec4d m_minEndCol{ 0.0 };
		Vec4d m_maxEndCol{ 0.0 };
	public:
		BasicColorGen() { }

		virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
	};

	class BasicVelGen : public ParticleGenerator
	{
	public:
		Vec4d m_minStartVel{ 0.0 };
		Vec4d m_maxStartVel{ 0.0 };
	public:
		BasicVelGen() { }

		virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
	};

	class VelFromPosGen : public ParticleGenerator
	{
	public:
		Vec4d m_offset{ 0.0 };
		FPType m_minScale{ 0.0 };
		FPType m_maxScale{ 0.0 };
	public:
		VelFromPosGen() { }
		VelFromPosGen(const Vec4d &off, double minS, double maxS)
			: m_offset(off)
			, m_minScale((FPType)minS)
			, m_maxScale((FPType)maxS)
		{ }

		virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
	};

	class BasicTimeGen : public ParticleGenerator
	{
	public:
		FPType m_minTime{ 0.0 };
		FPType m_maxTime{ 0.0 };
	public:
		BasicTimeGen() { }

		virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
	};

}