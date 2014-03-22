#pragma once

#include <memory>

class IEffect;

class EffectFactory
{
public:
	static std::shared_ptr<IEffect> create(const char *name);
};