#pragma once

#include <vector>
#include <memory>
#include "commonMath.h"

typedef TVec4<double> Vec4d;

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

	//void reset(size_t idStart, size_t idEnd) { m_idStart = idStart; m_idEnd = idEnd; }

	//virtual void reinit(ParticleData *) { }

	virtual void update(double dt, ParticleData *p) = 0;
};

class BasicParticleEmitter : public ParticleUpdater
{
public:
	double m_emitRate{ 0.0 };
	Vec4d m_pos{ 0.0 };
	Vec4d m_maxStartPosOffset{ 0.0 };

	Vec4d m_minStartCol{ 0.0 };
	Vec4d m_maxStartCol{ 0.0 };
	Vec4d m_minEndCol{ 0.0 };
	Vec4d m_maxEndCol{ 0.0 };

	double m_minTime{ 0.0 };
	double m_maxTime{ 0.0 };

	Vec4d m_minStartVel{ 0.0 };
	Vec4d m_maxStartVel{ 0.0 };
public:
	BasicParticleEmitter(unsigned int idStart, unsigned int idEnd) : BasicParticleEmitter{ idStart, idEnd } { }

	virtual void update(double dt, ParticleData *p) override;
};

class EulerParticleUpdater : public ParticleUpdater
{
public:
	Vec4d m_globalAcceleration;
public:
	EulerParticleUpdater(unsigned int idStart, unsigned int idEnd);

	virtual void update(double dt, ParticleData *p) override;
};

class BasicColorParticleUpdater : public ParticleUpdater
{
public:
	BasicColorParticleUpdater(unsigned int idStart, unsigned int idEnd) : ParticleUpdater(idStart, idEnd) { }

	virtual void update(double dt, ParticleData *p) override
	{
		for (unsigned int i = m_idStart; i < m_idEnd; ++i)
		{
			Vec4d::mix4(p->m_startCol[i], p->m_endCol[i], p->m_time[i].z, p->m_col[i]);
		}
	}
};

class BasicTimeParticleUpdater : public ParticleUpdater
{
public:
	BasicTimeParticleUpdater(unsigned int idStart, unsigned int idEnd) : ParticleUpdater(idStart, idEnd) { }

	virtual void update(double dt, ParticleData *p) override
	{
		for (unsigned int i = m_idStart; i < m_idEnd; ++i)
		{
			p->m_time[i].x -= dt;
			// interpolation: from 0 (start of life) till 1 (end of life)
			p->m_time[i].z = 1.0 - (p->m_time[i].x*p->m_time[i].w); // .w is max life time
			p->m_alive[i] = p->m_alive[i] && (p->m_time[i].x > 0.0);
		}
	}
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

	virtual void update(double dt);

	virtual size_t numAllParticles() const { return m_count; }
	virtual size_t numAliveParticles() const { return m_particles.m_countAlive; }

	//void addAttractor(const Vec4d &v) { m_attractors.add(v); }
	//Vec4d *getAttractor(int id) { return &m_attractors[id]; }

	void addEmitter(std::shared_ptr<ParticleUpdater> em) { m_emitters.push_back(em); }
	const ParticleUpdater *getEmitter(size_t id) const { return m_emitters[id].get(); }

	void addUpdater(std::shared_ptr<ParticleUpdater> up) { m_updaters.push_back(up); }
	const ParticleUpdater *getUpdater(size_t id) const { return m_emitters[id].get(); }

	ParticleData *finalData() { return &m_aliveParticles; }

	static size_t computeMemoryUsage(const ParticleSystem &p);
};