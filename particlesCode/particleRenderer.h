#pragma once

#include <memory>

namespace particles
{

	class ParticleSystem;

	class IParticleRenderer
	{
	public:
		IParticleRenderer() { }
		virtual ~IParticleRenderer() { }

		virtual void generate(ParticleSystem *sys, bool useQuads) = 0;
		virtual void destroy() = 0;
		virtual void update() = 0;
		virtual void render() = 0;
	};

	class RendererFactory
	{
	public:
		static std::shared_ptr<IParticleRenderer> create(const char *name);
	};
}