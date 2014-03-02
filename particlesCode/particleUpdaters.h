#pragma once

#include <vector>
#include "commonMath.h"
#include "particles.h"

namespace particleUpdaters
{

	class EulerUpdater : public ParticleUpdater
	{
	public:
		Vec4d m_globalAcceleration{ 0.0f };
	public:
		EulerUpdater() { }

		virtual void update(double dt, ParticleData *p) override;
	};

	class BasicColorUpdater : public ParticleUpdater
	{
	public:
		BasicColorUpdater() { }

		virtual void update(double dt, ParticleData *p) override;
	};

	class BasicTimeUpdater : public ParticleUpdater
	{
	public:
		BasicTimeUpdater() { }

		virtual void update(double dt, ParticleData *p) override;
	};
}