#pragma once

#include "particleRenderer.h"

namespace particles
{
	class GLParticleRenderer : public IParticleRenderer
	{
	protected:
		ParticleSystem *m_system{ nullptr };

		unsigned int m_bufPos{ 0 };
		unsigned int m_bufCol{ 0 };
		unsigned int m_vao{ 0 };
	public:
		GLParticleRenderer() { }
		~GLParticleRenderer() { destroy(); }

		void generate(ParticleSystem *sys, bool useQuads) override;
		void destroy() override;
		void update() override;
		void render() override;
	};
}