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
		particleEmitter->m_emitRate = (float)NUM_PARTICLES*0.1f;

		// pos:
		//auto posGenerator = std::make_shared<RoundPosGen>();
		gPosGenerator = std::make_shared<particles::generators::BoxPosGen>();
		gPosGenerator->m_pos = glm::vec4{ 0.0, 0.0, -0.25, 0.0 };
		gPosGenerator->m_maxStartPosOffset = glm::vec4{ 0.0, 0.0, 0.0, 0.0 };
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

		auto velGenerator = std::make_shared<particles::generators::SphereVelGen>();
		velGenerator->m_minVel = 0.1f;
		velGenerator->m_maxVel = 0.1f;
		particleEmitter->addGenerator(velGenerator);

		auto timeGenerator = std::make_shared<particles::generators::BasicTimeGen>();
		timeGenerator->m_minTime = 1.0;
		timeGenerator->m_maxTime = 3.5;
		particleEmitter->addGenerator(timeGenerator);
	}
	gParticleSystem->addEmitter(particleEmitter);

	auto particleEmitter2 = std::make_shared<particles::ParticleEmitter>();
	{
		particleEmitter2->m_emitRate = (float)NUM_PARTICLES*0.1f;

		// pos:
		//auto posGenerator = std::make_shared<RoundPosGen>();
		gPosGenerator2 = std::make_shared<particles::generators::BoxPosGen>();
		gPosGenerator2->m_pos = glm::vec4{ 0.0, 0.0, 0.25, 0.0 };
		gPosGenerator2->m_maxStartPosOffset = glm::vec4{ 0.0, 0.0, 0.0, 0.0 };
		//auto posGenerator = std::make_shared<particleGenerators::BoxPosGen>();
		//posGenerator->m_pos = glm::vec4{ 0.0, 0.0, 0.0, 0.0 };
		//posGenerator->m_maxStartPosOffset = glm::vec4{ 0.1, 0.1, 0.0, 0.0 };
		particleEmitter2->addGenerator(gPosGenerator2);

		/*gColGenerator = std::make_shared<particles::generators::BasicColorGen>();
		gColGenerator->m_minStartCol = glm::vec4{ 0.7, 0.0, 0.7, 1.0 };
		gColGenerator->m_maxStartCol = glm::vec4{ 1.0, 1.0, 1.0, 1.0 };
		gColGenerator->m_minEndCol = glm::vec4{ 0.5, 0.0, 0.6, 0.0 };
		gColGenerator->m_maxEndCol = glm::vec4{ 0.7, 0.5, 1.0, 0.0 };*/
		particleEmitter2->addGenerator(gColGenerator);

		auto velGenerator = std::make_shared<particles::generators::SphereVelGen>();
		velGenerator->m_minVel = 0.05f;
		velGenerator->m_maxVel = 0.05f;
		particleEmitter2->addGenerator(velGenerator);

		auto timeGenerator = std::make_shared<particles::generators::BasicTimeGen>();
		timeGenerator->m_minTime = 2.0;
		timeGenerator->m_maxTime = 3.9;
		particleEmitter2->addGenerator(timeGenerator);
	}
	gParticleSystem->addEmitter(particleEmitter2);

	auto particleEmitter3 = std::make_shared<particles::ParticleEmitter>();
	{
		particleEmitter3->m_emitRate = (float)NUM_PARTICLES*0.1f;

		// pos:
		//auto posGenerator = std::make_shared<RoundPosGen>();
		gPosGenerator3 = std::make_shared<particles::generators::BoxPosGen>();
		gPosGenerator3->m_pos = glm::vec4{ 0.0, 0.0, 0.25, 0.0 };
		gPosGenerator3->m_maxStartPosOffset = glm::vec4{ 0.0, 0.0, 0.0, 0.0 };
		//auto posGenerator = std::make_shared<particleGenerators::BoxPosGen>();
		//posGenerator->m_pos = glm::vec4{ 0.0, 0.0, 0.0, 0.0 };
		//posGenerator->m_maxStartPosOffset = glm::vec4{ 0.1, 0.1, 0.0, 0.0 };
		particleEmitter3->addGenerator(gPosGenerator3);

		/*gColGenerator = std::make_shared<particles::generators::BasicColorGen>();
		gColGenerator->m_minStartCol = glm::vec4{ 0.7, 0.0, 0.7, 1.0 };
		gColGenerator->m_maxStartCol = glm::vec4{ 1.0, 1.0, 1.0, 1.0 };
		gColGenerator->m_minEndCol = glm::vec4{ 0.5, 0.0, 0.6, 0.0 };
		gColGenerator->m_maxEndCol = glm::vec4{ 0.7, 0.5, 1.0, 0.0 };*/
		particleEmitter3->addGenerator(gColGenerator);

		auto velGenerator = std::make_shared<particles::generators::SphereVelGen>();
		velGenerator->m_minVel = 0.05f;
		velGenerator->m_maxVel = 0.05f;
		particleEmitter3->addGenerator(velGenerator);

		auto timeGenerator = std::make_shared<particles::generators::BasicTimeGen>();
		timeGenerator->m_minTime = 2.0;
		timeGenerator->m_maxTime = 3.9;
		particleEmitter3->addGenerator(timeGenerator);
	}
	gParticleSystem->addEmitter(particleEmitter3);

	auto timeUpdater = std::make_shared<particles::BasicTimeUpdater>();
	gParticleSystem->addUpdater(timeUpdater);

	auto colorUpdater = std::make_shared<particles::BasicColorUpdater>();
	gParticleSystem->addUpdater(colorUpdater);

	auto attractorUpdater = std::make_shared<particles::AttractorUpdater>();
	attractorUpdater->add(glm::vec4 {0.0, 0.0, 0.75, 1.0});
	attractorUpdater->add(glm::vec4 { 0.0, 0.0, -0.75, 1.0 });
	attractorUpdater->add(glm::vec4{ 0.0, 0.75, 0.0, 1.0 });
	attractorUpdater->add(glm::vec4{ 0.0, -0.75, 0.0, 1.0 });
	gParticleSystem->addUpdater(attractorUpdater);

	auto eulerUpdater = std::make_shared<particles::EulerUpdater>();
	eulerUpdater->m_globalAcceleration = glm::vec4{ 0.0, 0.0, 0.0, 0.0 };
	gParticleSystem->addUpdater(eulerUpdater);

	//auto myUpdater = std::make_shared<MyUpdater>(0, NUM_PARTICLES);
	//gParticleSystem->addUpdater(myUpdater);

	gParticleRenderer = std::make_shared<particles::GLParticleRenderer>();
	gParticleRenderer->generate(gParticleSystem.get(), false);

	m_zScale = 1.0f;

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
	ui::AddTweak("z scale", &m_zScale, "min=0.0 max=1.0 step=0.05 group=effect");
}

void AttractorEffect::removeUI()
{
	ui::RemoveVar("start col min");
	ui::RemoveVar("start col max");
	ui::RemoveVar("end col min");
	ui::RemoveVar("end col max");
	ui::RemoveVar("z scale");
}

void AttractorEffect::update(double dt)
{
	static double time = 0.0;
	time += dt;

	gPosGenerator->m_pos.x = 0.15*sinf((float)time*2.5);
	gPosGenerator->m_pos.y = 0.15*cosf((float)time*2.5);
	gPosGenerator->m_pos.z = m_zScale*0.25*cosf((float)time*2.5);
	//gPosGenerator->m_maxStartPosOffset.x = 0.15f + 0.05*sinf((float)time);
	//gPosGenerator->m_maxStartPosOffset.y = 0.15f + 0.05*sinf((float)time)*cosf((float)time*0.5);

	gPosGenerator2->m_pos.x = -0.15*sinf((float)time*2.);
	gPosGenerator2->m_pos.y = 0.15*cosf((float)time*2.);
	gPosGenerator2->m_pos.z = m_zScale*0.25*cosf((float)time*1.5);

	gPosGenerator3->m_pos.x = -0.15*sinf((float)time*1.5);
	gPosGenerator3->m_pos.y = 0.15*cosf((float)time*2.5);
	gPosGenerator3->m_pos.z = m_zScale*0.25*cosf((float)time*1.75);
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