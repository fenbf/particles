/** @file shaderLoader.cpp
*  @brief useful functions for shader management
*
*	@author Bartlomiej Filipek 
*/

#include "commonCode.h"

#include "Log.h"
#include "TextureLoader.h"
#include "SOIL2.h"

#pragma comment(lib, "soil2")

namespace textureLoader
{
    bool loadTexture(const char *filename, GLuint *outTexId)
    {
        assert(outTexId);
        *outTexId = SOIL_load_OGL_texture
        (
        filename,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        /*SOIL_FLAG_GL_MIPMAPS | */SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS
        );

        if (*outTexId == 0)
        {
            LOG_ERROR("Cannot load texture: %s", filename);
        }

        return *outTexId != 0;
    }

} // namespce textureLoader