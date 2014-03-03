#pragma once

#include <utility>
#include "particles.h"
#include "particleGenerators.h"
#include "particleRenderer.h"
#include "particleUpdaters.h"

class IEffect
{
public:
	//enum Name { };
public:
	IEffect() { }
	virtual ~IEffect() { }

	virtual bool initialize() = 0;
	virtual void clean() = 0;
	virtual void addUI() = 0;
	virtual void removeUI() = 0;
	
	virtual void update(double dt) = 0;
	virtual void cpuUpdate(double dt) = 0;
	virtual void gpuUpdate(double dt) = 0;
	virtual void render() = 0;

	//virtual int num
};

class TunnelEffect : public IEffect
{
private:
	std::shared_ptr<particles::ParticleSystem> gParticleSystem;
	std::shared_ptr<particles::GLParticleRenderer> gParticleRenderer;
	std::shared_ptr<particles::generators::RoundPosGen> gPosGenerator;
	std::shared_ptr<particles::generators::BasicColorGen> gColGenerator;
public:
	TunnelEffect() { }
	 ~TunnelEffect() { }

	bool initialize() override;
	void clean() override;
	void addUI() override;
	void removeUI() override;

	void update(double dt) override;
	void cpuUpdate(double dt) override;
	void gpuUpdate(double dt) override;
	void render() override;
};