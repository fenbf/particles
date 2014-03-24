#pragma once

#include <memory>

class IEffect
{
public:
	static const size_t DEFAULT_PARTICLE_NUM_FLAG = 0;	// for initialize method
	//enum Name { };
public:
	IEffect() { }
	virtual ~IEffect() { }

	// creates the effect with desired num of particles, (0 means default for the effect)
	virtual bool initialize(size_t numParticles) = 0;
	virtual bool initializeRenderer() = 0;
	virtual void reset() = 0;
	virtual void clean() = 0;
	virtual void addUI() = 0;
	virtual void removeUI() = 0;
	
	virtual void update(double dt) = 0;
	virtual void cpuUpdate(double dt) = 0;
	virtual void gpuUpdate(double dt) = 0;
	virtual void render() = 0;

	virtual int numAllParticles() = 0;
	virtual int numAliveParticles() = 0;
};

class EffectFactory
{
public:
	static std::shared_ptr<IEffect> create(const char *name);
};
