#pragma once

#include <vector>
#include "commonMath.h"
#include "particles.h"

class EulerParticleUpdater : public ParticleUpdater
{
public:
	Vec4d m_globalAcceleration{ 0.0f };
public:
	EulerParticleUpdater() { }

	virtual void update(double dt, ParticleData *p) override;
};

class BasicColorParticleUpdater : public ParticleUpdater
{
public:
	BasicColorParticleUpdater() { }

	virtual void update(double dt, ParticleData *p) override;
};

class BasicTimeParticleUpdater : public ParticleUpdater
{
public:
	BasicTimeParticleUpdater() { }

	virtual void update(double dt, ParticleData *p) override;
};