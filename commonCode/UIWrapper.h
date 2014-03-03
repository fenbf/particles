/** @file uiwrapper.h
*  @brief wrapper for ui system
*
*	@author Bartlomiej Filipek 
*/

#pragma once

#ifdef _EX_USE_ANTTWEAKBAR
    #include <AntTweakBar.h>
#endif

namespace ui
{
#ifdef _EX_USE_ANTTWEAKBAR
	template <typename T> struct TypeToTW
	{
		static const TwType value{ TW_TYPE_UNDEF };
	};

	template <>	struct TypeToTW<int> { static const TwType value{ TW_TYPE_INT32 }; };
	template <>	struct TypeToTW<bool> { static const TwType value{ TW_TYPE_BOOLCPP }; };
	template <>	struct TypeToTW<float> { static const TwType value{ TW_TYPE_FLOAT }; };
	template <>	struct TypeToTW<double> { static const TwType value{ TW_TYPE_DOUBLE }; };

	template <typename T> void AddTweak(const char *name, T *var, const char *def)
	{
		TwAddVarRW(Globals::MainTweakBar, name, TypeToTW<T>::value, var, def);
	}

	inline void AddTweakColor3f(const char *name, float *col, const char *def)
	{
		TwAddVarRW(Globals::MainTweakBar, name, TW_TYPE_COLOR3F, col, def);
	}

	inline void AddTweakColor4f(const char *name, float *col, const char *def)
	{
		TwAddVarRW(Globals::MainTweakBar, name, TW_TYPE_COLOR4F, col, def);
	}

	inline void AddTweakDir3f(const char *name, float *dir, const char *def)
	{
		TwAddVarRW(Globals::MainTweakBar, name, TW_TYPE_DIR3F, dir, def);
	}
    
	template <typename T> void AddVar(const char *name, T *var, const char *def)
	{
		TwAddVarRO(Globals::MainTweakBar, name, TypeToTW<T>::value, var, def);
	}

	inline void RemoveVar(const char *name)
	{
		TwRemoveVar(Globals::MainTweakBar, name);
	}

	inline void RemoveAllVars()
	{
		TwRemoveAllVars(Globals::MainTweakBar);
	}

#else
	inline template <typename T> void AddTweak(const char *name, T *var, const char *def) { }
	inline void AddTweakColor3f(const char *name, float *col, const char *def) { }
	inline void AddTweakColor4f(const char *name, float *col, const char *def) { }
	inline void AddTweakDir3f(const char *name, float *dir, const char *def) { }
	inline template <typename T> void AddVar(const char *name, T *var, const char *def) { }
	inline void RemoveVar(const char *name) { }
	inline void RemoveAllVars() { }
#endif
}
