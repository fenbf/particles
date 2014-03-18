#pragma once

#include "effect.h"
#include <utility>
#include "particles.h"
#include "particleGenerators.h"
#include "particleRenderer.h"
#include "particleUpdaters.h"


class FountainEffect : public IEffect
{
private:
	std::shared_ptr<particles::ParticleSystem> m_system;
	//std::shared_ptr<particles::ParticleSystem> m_system;
	std::shared_ptr<particles::GLParticleRenderer> m_renderer;
	std::shared_ptr<particles::generators::BoxPosGen> m_posGenerator;
	std::shared_ptr<particles::generators::BasicColorGen> m_colGenerator;
	std::shared_ptr<particles::updaters::EulerUpdater> m_eulerUpdater;
	std::shared_ptr<particles::updaters::FloorUpdater> m_floorUpdater;
public:
	FountainEffect() { }
	~FountainEffect() { }

	bool initialize() override;
	void reset() override { m_system->reset(); }
	void clean() override;
	void addUI() override;
	void removeUI() override;

	void update(double dt) override;
	void cpuUpdate(double dt) override;
	void gpuUpdate(double dt) override;
	void render() override;

	int numAllParticles() override { return m_system->numAllParticles(); }
	int numAliveParticles() override { return m_system->numAliveParticles(); }
};