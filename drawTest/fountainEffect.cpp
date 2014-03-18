#include "fountainEffect.h"
#include "main.h"
#include "UIWrapper.h"

bool FountainEffect::initialize()
{
	//
	// particles
	//
	const size_t NUM_PARTICLES = 10000;
	m_system = std::make_shared<particles::ParticleSystem>(NUM_PARTICLES);

	//
	// emitter:
	//
	auto particleEmitter = std::make_shared<particles::ParticleEmitter>();
	{
		particleEmitter->m_emitRate = (float)NUM_PARTICLES*0.25f;

		// pos:
		m_posGenerator = std::make_shared<particles::generators::BoxPosGen>();
		m_posGenerator->m_pos = glm::vec4{ 0.0, 0.0, 0.0, 0.0 };
		m_posGenerator->m_maxStartPosOffset = glm::vec4{ 0.0, 0.0, 0.0, 0.0 };
		particleEmitter->addGenerator(m_posGenerator);

		m_colGenerator = std::make_shared<particles::generators::BasicColorGen>();
		m_colGenerator->m_minStartCol = glm::vec4{ 0.7, 0.7, 0.7, 1.0 };
		m_colGenerator->m_maxStartCol = glm::vec4{ 1.0, 1.0, 1.0, 1.0 };
		m_colGenerator->m_minEndCol = glm::vec4{ 0.5, 0.0, 0.6, 0.0 };
		m_colGenerator->m_maxEndCol = glm::vec4{ 0.7, 0.5, 1.0, 0.0 };
		particleEmitter->addGenerator(m_colGenerator);

		auto velGenerator = std::make_shared<particles::generators::BasicVelGen>();
		velGenerator->m_minStartVel = glm::vec4{ -0.05f, 0.22f, -0.05f, 0.0f };
		velGenerator->m_maxStartVel = glm::vec4{ 0.05f, 0.25f, 0.05f, 0.0f };
		particleEmitter->addGenerator(velGenerator);

		auto timeGenerator = std::make_shared<particles::generators::BasicTimeGen>();
		timeGenerator->m_minTime = 3.0f;
		timeGenerator->m_maxTime = 4.0f;
		particleEmitter->addGenerator(timeGenerator);
	}
	m_system->addEmitter(particleEmitter);

	auto timeUpdater = std::make_shared<particles::updaters::BasicTimeUpdater>();
	m_system->addUpdater(timeUpdater);

	auto colorUpdater = std::make_shared<particles::updaters::BasicColorUpdater>();
	m_system->addUpdater(colorUpdater);

	m_eulerUpdater = std::make_shared<particles::updaters::EulerUpdater>();
	m_eulerUpdater->m_globalAcceleration = glm::vec4{ 0.0, -15.0, 0.0, 0.0 };
	m_system->addUpdater(m_eulerUpdater);

	m_floorUpdater = std::make_shared<particles::updaters::FloorUpdater>();
	m_system->addUpdater(m_floorUpdater);

	m_renderer = std::make_shared<particles::GLParticleRenderer>();
	m_renderer->generate(m_system.get(), false);

	return true;
}

void FountainEffect::clean()
{
	if (m_renderer) m_renderer->destroy();
}

void FountainEffect::addUI()
{
	ui::AddTweakColor4f("start col min", &m_colGenerator->m_minStartCol.x, "group=effect");
	ui::AddTweakColor4f("start col max", &m_colGenerator->m_maxStartCol.x, "group=effect");
	ui::AddTweakColor4f("end col min", &m_colGenerator->m_minEndCol.x, "group=effect");
	ui::AddTweakColor4f("end col max", &m_colGenerator->m_maxEndCol.x, "group=effect");
	ui::AddTweak("gravity", &m_eulerUpdater->m_globalAcceleration.y, "group=effect min=-20 max=0 step=0.05");
	ui::AddTweak("bounce", &m_floorUpdater->m_bounceFactor, "group=effect min=0 max=1 step=0.05");
}

void FountainEffect::removeUI()
{
	ui::RemoveVar("start col min");
	ui::RemoveVar("start col max");
	ui::RemoveVar("end col min");
	ui::RemoveVar("end col max");
	ui::RemoveVar("gravity");
	ui::RemoveVar("bounce");
}

void FountainEffect::update(double dt)
{
	static double time = 0.0;
	time += dt;

	m_posGenerator->m_pos.x = 0.1f*sinf((float)time*2.5f);
	m_posGenerator->m_pos.z = 0.1f*cosf((float)time*2.5f);
}

void FountainEffect::cpuUpdate(double dt)
{
	m_system->update(dt);
}

void FountainEffect::gpuUpdate(double dt)
{
	m_renderer->update();
}

void FountainEffect::render()
{
	m_renderer->render();
}