#pragma once

#include <vector>
#include <memory>
#include <glm/vec4.hpp>

typedef glm::vec4 Vec4d;
typedef glm::vec4::value_type FPType;

class ParticleData
{
public:
	std::unique_ptr<Vec4d[]> m_pos;
	std::unique_ptr<Vec4d[]> m_col;
	std::unique_ptr<Vec4d[]> m_startCol;
	std::unique_ptr<Vec4d[]> m_endCol;
	std::unique_ptr<Vec4d[]> m_vel;
	std::unique_ptr<Vec4d[]> m_acc;
	std::unique_ptr<Vec4d[]> m_time;
	std::unique_ptr<bool[]>  m_alive;

	size_t m_count{ 0 };
	size_t m_countAlive{ 0 };
public:
	ParticleData() { }
	explicit ParticleData(size_t maxCount) { generate(maxCount); }
	~ParticleData() { }

	ParticleData(const ParticleData &) = delete;
	ParticleData &operator=(const ParticleData &) = delete;

	void generate(size_t maxSize);
	void kill(size_t id);
	void wake(size_t id);
	void swapData(size_t a, size_t b);

	static void copyOnlyAlive(const ParticleData *source, ParticleData *destination);
	static size_t computeMemoryUsage(const ParticleData &p);
};

class ParticleGenerator
{
public:
	ParticleGenerator() { }
	virtual ~ParticleGenerator() { }

	virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) = 0;
};

class ParticleEmitter
{
protected:
	std::vector<std::shared_ptr<ParticleGenerator>> m_generators;
public:
	FPType m_emitRate{ 0.0 };	
public:
	ParticleEmitter() { }
	virtual ~ParticleEmitter() { }

	// calls all the generators and at the end it activates (wakes) particle
	virtual void emit(double dt, ParticleData *p);

	void addGenerator(std::shared_ptr<ParticleGenerator> gen) { m_generators.push_back(gen); }
};

class ParticleUpdater
{
public:
	ParticleUpdater() { }
	virtual ~ParticleUpdater() { }

	virtual void update(double dt, ParticleData *p) = 0;
};

class ParticleSystem
{
protected:
	ParticleData m_particles;
	ParticleData m_aliveParticles;

	size_t m_count;

	std::vector<std::shared_ptr<ParticleEmitter>> m_emitters;
	std::vector<std::shared_ptr<ParticleUpdater>> m_updaters;

public:
	explicit ParticleSystem(size_t maxCount);
	virtual ~ParticleSystem() { }

	ParticleSystem(const ParticleSystem &) = delete;
	ParticleSystem &operator=(const ParticleSystem &) = delete;

	virtual void update(double dt);

	virtual size_t numAllParticles() const { return m_count; }
	virtual size_t numAliveParticles() const { return m_particles.m_countAlive; }

	void addEmitter(std::shared_ptr<ParticleEmitter> em) { m_emitters.push_back(em); }
	void addUpdater(std::shared_ptr<ParticleUpdater> up) { m_updaters.push_back(up); }

	ParticleData *finalData() { return &m_particles; }

	static size_t computeMemoryUsage(const ParticleSystem &p);
};