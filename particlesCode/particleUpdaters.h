#pragma once

#include <vector>
#include "commonMath.h"
#include "particles.h"

namespace particles
{

	class EulerUpdater : public particles::ParticleUpdater
	{
	public:
		glm::vec4 m_globalAcceleration{ 0.0f };
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