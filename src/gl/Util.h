#ifndef __MEDLEAP_GL_UTIL__
#define __MEDLEAP_GL_UTIL__

#include "GL/glew.h"

namespace gl
{
    int sizeOf(GLenum typeEnum);
    
    void flipImage(void* buffer, int width, int height, int pixelSize);
    
} // namespace cgl

#endif // __MEDLEAP_GL_UTIL__