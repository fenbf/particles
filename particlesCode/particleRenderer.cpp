#include "particleRenderer.h"
#include "glParticleRenderer.h"

#include <string>
#include <map>

template <typename T> std::shared_ptr<particles::IParticleRenderer> createRenderer() { return std::make_shared<T>(); }

namespace particles
{
	std::unique_ptr<std::map<std::string, std::shared_ptr<IParticleRenderer>(*)()>> RendererFactory::s_generatorMap;

	void RendererFactory::initGeneratorMap()
	{
		if (s_generatorMap)
			return;

		s_generatorMap = std::make_unique<std::map<std::string, std::shared_ptr<IParticleRenderer>(*)()>>();
		(*s_generatorMap)["gl"] = &createRenderer<GLParticleRenderer>;
		(*s_generatorMap)["gl_map"] = &createRenderer<GLParticleRendererUseMap>;
		(*s_generatorMap)["gl_double"] = &createRenderer<GLParticleRendererDoubleVao>;
		(*s_generatorMap)["gl_persistent"] = &createRenderer<GLParticleRendererPersistent>;
	}

	std::shared_ptr<IParticleRenderer> RendererFactory::create(const char *name)
	{
		if (!s_generatorMap)
			initGeneratorMap();

		if (isAvailable(name))
			return (*s_generatorMap)[name]();

		return nullptr;
	}

	bool RendererFactory::isAvailable(const char *name)
	{
		if (!s_generatorMap)
			initGeneratorMap();

		return (s_generatorMap->find(name) != s_generatorMap->end());
	}



}

