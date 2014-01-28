#pragma once

#include <vector>
#include <memory>
#include "commonMath.h"

typedef float FPType;
typedef TVec4<FPType> Vec4d;

class ParticleData
{
public:
	std::vector<Vec4d> m_pos;
	std::vector<Vec4d> m_col;
	std::vector<Vec4d> m_startCol;
	std::vector<Vec4d> m_endCol;	
	std::vector<Vec4d> m_vel;
	std::vector<Vec4d> m_acc;
	std::vector<Vec4d> m_time;
	std::vector<bool> m_alive;

	size_t m_count = 0;
	size_t m_countAlive = 0;
public:
	ParticleData() { }
	explicit ParticleData(size_t maxCount) { generate(maxCount); }
	~ParticleData() { }

	ParticleData(const ParticleData &) = delete;
	ParticleData &operator=(const ParticleData &) = delete;

	void generate(size_t maxSize);

	static void copyOnlyAlive(const ParticleData *source, ParticleData *destination);
	static size_t computeMemoryUsage(const ParticleData &p);
};

class ParticleUpdater
{
protected:
	size_t m_idStart = 0;
	size_t m_idEnd = 0;
public:
	ParticleUpdater(size_t idStart, size_t idEnd) { m_idStart = idStart; m_idEnd = idEnd; }
	virtual ~ParticleUpdater() { }

	virtual void update(FPType dt, ParticleData *p) = 0;
};

class ParticleSystem
{
protected:
	ParticleData m_particles;
	ParticleData m_aliveParticles;

	size_t m_count;

	std::vector<std::shared_ptr<ParticleUpdater>> m_emitters;
	std::vector<std::shared_ptr<ParticleUpdater>> m_updaters;

public:
	explicit ParticleSystem(size_t maxCount);
	virtual ~ParticleSystem() { }

	ParticleSystem(const ParticleSystem &) = delete;
	ParticleSystem &operator=(const ParticleSystem &) = delete;

	virtual void update(FPType dt);

	virtual size_t numAllParticles() const { return m_count; }
	virtual size_t numAliveParticles() const { return m_aliveParticles.m_countAlive; }

	void addEmitter(std::shared_ptr<ParticleUpdater> em) { m_emitters.push_back(em); }
	void addUpdater(std::shared_ptr<ParticleUpdater> up) { m_updaters.push_back(up); }

	ParticleData *finalData() { return &m_aliveParticles; }

	static size_t computeMemoryUsage(const ParticleSystem &p);
};