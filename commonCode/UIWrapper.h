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
    void AddTweakInt(const char *name, int *var, const char *def);
    void AddTweakBool(const char *name, bool *var, const char *def);
    void AddTweakFloat(const char *name, float *var, const char *def);
    void AddTweakColor3f(const char *name, float *col, const char *def);
    void AddTweakColor4f(const char *name, float *col, const char *def);
    void AddTweakDir3f(const char *name, float *dir, const char *def);
    // todo: use templates?
#else
    inline void AddTweakInt(const char *, int *, const char *) { }
    inline void AddTweakBool(const char *, bool *, const char *) { }
    inline void AddTweakFloat(const char *, float *, const char *) { }
    inline void AddTweakColor3f(const char *, float *, const char *) { }
    inline void AddTweakColor4f(const char *, float *, const char *) { }
    inline void AddTweakDir3f(const char *, float *, const char *) { }
#endif
}
