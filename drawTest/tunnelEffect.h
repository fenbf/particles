#pragma once

#include "effect.h"
#include <utility>
#include "particles.h"
#include "particleGenerators.h"
#include "particleRenderer.h"
#include "particleUpdaters.h"


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

	int numAllParticles() override { return gParticleSystem->numAllParticles(); }
	int numAliveParticles() override { return gParticleSystem->numAliveParticles(); }
};