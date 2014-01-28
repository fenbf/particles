/** @file uiwrapper.h
*  @brief wrapper for ui system
*
*	@author Bartlomiej Filipek 
*/

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
    
	// todo: use templates?
	template <typename T> void AddVar(const char *name, T *var, const char *def)
	{
		TwAddVarRO(Globals::MainTweakBar, name, TypeToTW<T>::value, var, def);
	}

#else
    inline void AddTweakInt(const char *, int *, const char *) { }
    inline void AddTweakBool(const char *, bool *, const char *) { }
    inline void AddTweakFloat(const char *, float *, const char *) { }
	inline void AddTweakDouble(const char *name, double *var, const char *def) { }
    inline void AddTweakColor3f(const char *, float *, const char *) { }
    inline void AddTweakColor4f(const char *, float *, const char *) { }
    inline void AddTweakDir3f(const char *, float *, const char *) { }
	
	inline template <typename T> void AddReadonlyVar(const char *name, T *var, const char *def) { }
#endif
}
