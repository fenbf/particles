#pragma once

#include "particleRenderer.h"

namespace particles
{
	class GLParticleRenderer : public IParticleRenderer
	{
	protected:
		ParticleSystem *m_system{ nullptr };

		unsigned int m_bufPos;
		unsigned int m_bufCol;
		unsigned int m_vao;
	public:
		GLParticleRenderer() { }
		~GLParticleRenderer() { destroy(); }

		virtual void generate(ParticleSystem *sys, bool useQuads) override;
		virtual void destroy() override;
		virtual void update() override;
		virtual void render() override;
	};

	class GLParticleRendererUseMap : public GLParticleRenderer
	{
	public:
		GLParticleRendererUseMap() { }
		~GLParticleRendererUseMap() { destroy(); }

		void update() override;
	};

	class GLParticleRendererDoubleVao : public GLParticleRenderer
	{
	protected:
		unsigned int m_doubleBufPos[2];
		unsigned int m_doubleBufCol[2];
		unsigned int m_doubleVao[2];
		unsigned int m_id;
	public:
		GLParticleRendererDoubleVao() { }
		~GLParticleRendererDoubleVao() { destroy(); }

		virtual void generate(ParticleSystem *sys, bool useQuads) override;
		virtual void destroy() override;
		virtual void update() override;
		virtual void render() override;
	};

	class GLParticleRendererPersistent : public GLParticleRenderer
	{
	protected:
		unsigned int m_id;
		float *m_mappedPosBuf = nullptr;
		float *m_mappedColBuf = nullptr;
	public:
		GLParticleRendererPersistent() { }
		~GLParticleRendererPersistent() { destroy(); }

		virtual void generate(ParticleSystem *sys, bool useQuads) override;
		virtual void update() override;
		virtual void render() override;
	};
}