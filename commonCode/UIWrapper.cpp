/** @file uiwrapper.cpp
*  @brief wrapper for ui system
*
*	@author Bartlomiej Filipek 
*/

#include "main.h"

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

    void AddTweakInt(const char *name, int *var, const char *def)
    {
        TwAddVarRW(Globals::MainTweakBar, name, TW_TYPE_INT32, var, def);
    }

    void AddTweakBool(const char *name, bool *var, const char *def)
    {
        TwAddVarRW(Globals::MainTweakBar, name, TW_TYPE_BOOLCPP, var, def);
    }

    void AddTweakFloat(const char *name, float *var, const char *def)
    {
        TwAddVarRW(Globals::MainTweakBar, name, TW_TYPE_FLOAT, var, def);
    }

	void AddTweakDouble(const char *name, double *var, const char *def)
	{
		TwAddVarRW(Globals::MainTweakBar, name, TW_TYPE_DOUBLE, var, def);
	}

    void AddTweakColor3f(const char *name, float *col, const char *def)
    {
        TwAddVarRW(Globals::MainTweakBar, name, TW_TYPE_COLOR3F, col, def);
    }

    void AddTweakColor4f(const char *name, float *col, const char *def)
    {
        TwAddVarRW(Globals::MainTweakBar, name, TW_TYPE_COLOR4F, col, def);
    }

    void AddTweakDir3f(const char *name, float *dir, const char *def)
    {
        TwAddVarRW(Globals::MainTweakBar, name, TW_TYPE_DIR3F, dir, def);
    }

	template <typename T> void AddReadonlyVar(const char *name, T *var, const char *def) 
	{
		
	}

	template <> void AddReadonlyVar<double>(const char *name, double *var, const char *def)
	{
		TwAddVarRO(Globals::MainTweakBar, name, TypeToTW<double>::value, var, def);
	}
	
#endif
}

