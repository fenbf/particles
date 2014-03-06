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

	class AttractorUpdater : public particles::ParticleUpdater
	{
	protected:
		std::vector<glm::vec4> m_attractors; // .w is force
	public:
		AttractorUpdater() { }

		virtual void update(double dt, ParticleData *p) override;
		void add(const glm::vec4 &attr) { m_attractors.push_back(attr); }
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