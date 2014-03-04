#include "attractorEffect.h"
#include "main.h"
#include "UIWrapper.h"

bool AttractorEffect::initialize()
{
	//
	// particles
	//
	const size_t NUM_PARTICLES = 100000;
	gParticleSystem = std::make_shared<particles::ParticleSystem>(NUM_PARTICLES);

	//
	// emitter:
	//
	auto particleEmitter = std::make_shared<particles::ParticleEmitter>();
	{
		particleEmitter->m_emitRate = (float)NUM_PARTICLES*0.45f;

		// pos:
		//auto posGenerator = std::make_shared<RoundPosGen>();
		gPosGenerator = std::make_shared<particles::generators::BoxPosGen>();
		gPosGenerator->m_pos = glm::vec4{ 0.0, 0.0, 0.0, 0.0 };
		gPosGenerator->m_maxStartPosOffset = glm::vec4{ 0.1, 0.1, 0.1, 0.0 };
		//auto posGenerator = std::make_shared<particleGenerators::BoxPosGen>();
		//posGenerator->m_pos = glm::vec4{ 0.0, 0.0, 0.0, 0.0 };
		//posGenerator->m_maxStartPosOffset = glm::vec4{ 0.1, 0.1, 0.0, 0.0 };
		particleEmitter->addGenerator(gPosGenerator);

		gColGenerator = std::make_shared<particles::generators::BasicColorGen>();
		gColGenerator->m_minStartCol = glm::vec4{ 0.7, 0.0, 0.7, 1.0 };
		gColGenerator->m_maxStartCol = glm::vec4{ 1.0, 1.0, 1.0, 1.0 };
		gColGenerator->m_minEndCol = glm::vec4{ 0.5, 0.0, 0.6, 0.0 };
		gColGenerator->m_maxEndCol = glm::vec4{ 0.7, 0.5, 1.0, 0.0 };
		particleEmitter->addGenerator(gColGenerator);

		auto velGenerator = std::make_shared<particles::generators::BasicVelGen>();
		velGenerator->m_minStartVel = glm::vec4{ 0.0f, 0.0f, 0.15f, 0.0f };
		velGenerator->m_maxStartVel = glm::vec4{ 0.0f, 0.0f, 0.45f, 0.0f };
		particleEmitter->addGenerator(velGenerator);

		auto timeGenerator = std::make_shared<particles::generators::BasicTimeGen>();
		timeGenerator->m_minTime = 1.0;
		timeGenerator->m_maxTime = 3.5;
		particleEmitter->addGenerator(timeGenerator);
	}
	gParticleSystem->addEmitter(particleEmitter);

	auto timeUpdater = std::make_shared<particles::BasicTimeUpdater>();
	gParticleSystem->addUpdater(timeUpdater);

	auto colorUpdater = std::make_shared<particles::BasicColorUpdater>();
	gParticleSystem->addUpdater(colorUpdater);

	auto eulerUpdater = std::make_shared<particles::EulerUpdater>();
	eulerUpdater->m_globalAcceleration = glm::vec4{ 0.0, 0.0, 0.0, 0.0 };
	gParticleSystem->addUpdater(eulerUpdater);

	//auto myUpdater = std::make_shared<MyUpdater>(0, NUM_PARTICLES);
	//gParticleSystem->addUpdater(myUpdater);

	gParticleRenderer = std::make_shared<particles::GLParticleRenderer>();
	gParticleRenderer->generate(gParticleSystem.get(), false);

	return true;
}

void AttractorEffect::clean()
{
	if (gParticleRenderer) gParticleRenderer->destroy();
}

void AttractorEffect::addUI()
{
	ui::AddTweakColor4f("start col min", &gColGenerator->m_minStartCol.x, "group=effect");
	ui::AddTweakColor4f("start col max", &gColGenerator->m_maxStartCol.x, "group=effect");
	ui::AddTweakColor4f("end col min", &gColGenerator->m_minEndCol.x, "group=effect");
	ui::AddTweakColor4f("end col max", &gColGenerator->m_maxEndCol.x, "group=effect");
}

void AttractorEffect::removeUI()
{
	ui::RemoveVar("start col min");
	ui::RemoveVar("start col max");
	ui::RemoveVar("end col min");
	ui::RemoveVar("end col max");
}

void AttractorEffect::update(double dt)
{
	static double time = 0.0;
	time += dt;

	gPosGenerator->m_pos.x = 0.1*sinf((float)time*2.5);
	gPosGenerator->m_pos.y = 0.1*cosf((float)time*2.5);
	gPosGenerator->m_maxStartPosOffset.x = 0.15f + 0.05*sinf((float)time);
	gPosGenerator->m_maxStartPosOffset.y = 0.15f + 0.05*sinf((float)time)*cosf((float)time*0.5);
}

void AttractorEffect::cpuUpdate(double dt)
{
	gParticleSystem->update(dt);
}

void AttractorEffect::gpuUpdate(double dt)
{
	gParticleRenderer->update();
}

void AttractorEffect::render()
{
	gParticleRenderer->render();
}