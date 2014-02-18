#pragma once

#include <vector>
#include "commonMath.h"
#include "particles.h"

typedef glm::vec4::value_type FPType;

class BasicParticleEmitter : public ParticleUpdater
{
public:
	FPType m_emitRate{ 0.0 };
	Vec4d m_pos{ 0.0 };
	Vec4d m_maxStartPosOffset{ 0.0 };

	Vec4d m_minStartCol{ 0.0 };
	Vec4d m_maxStartCol{ 0.0 };
	Vec4d m_minEndCol{ 0.0 };
	Vec4d m_maxEndCol{ 0.0 };

	FPType m_minTime{ 0.0 };
	FPType m_maxTime{ 0.0 };

	Vec4d m_minStartVel{ 0.0 };
	Vec4d m_maxStartVel{ 0.0 };
public:
	BasicParticleEmitter(unsigned int idStart, unsigned int idEnd) : ParticleUpdater{ idStart, idEnd } { }

	virtual void update(double dt, ParticleData *p) override;
};

class EulerParticleUpdater : public ParticleUpdater
{
public:
	Vec4d m_globalAcceleration{ 0.0f };
public:
	EulerParticleUpdater(unsigned int idStart, unsigned int idEnd) : ParticleUpdater{ idStart, idEnd } { }

	virtual void update(double dt, ParticleData *p) override;
};

class BasicColorParticleUpdater : public ParticleUpdater
{
public:
	BasicColorParticleUpdater(unsigned int idStart, unsigned int idEnd) : ParticleUpdater(idStart, idEnd) { }

	virtual void update(double dt, ParticleData *p) override;
};

class BasicTimeParticleUpdater : public ParticleUpdater
{
public:
	BasicTimeParticleUpdater(unsigned int idStart, unsigned int idEnd) : ParticleUpdater(idStart, idEnd) { }

	virtual void update(double dt, ParticleData *p) override;
};