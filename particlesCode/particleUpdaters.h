#pragma once

#include <vector>
#include "commonMath.h"
#include "particles.h"



class BasicParticleEmitter : public ParticleEmitter
{
public:
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
	BasicParticleEmitter() { }

protected:
	// only generates positions for a particles
	virtual void generatePos(double dt, ParticleData *p, size_t startId, size_t endId) override;
	// only generates col, startCol and endCol
	virtual void generateCol(double dt, ParticleData *p, size_t startId, size_t endId) override;
	// generates rest of fields beside Time and Alive!
	virtual void generateOther(double dt, ParticleData *p, size_t startId, size_t endId) override;
	// generates time
	virtual void generateTime(double dt, ParticleData *p, size_t startId, size_t endId) override;
};

class 

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