#ifndef __MEDLEAP_GL_UTIL__
#define __MEDLEAP_GL_UTIL__

#include "gl/glew.h"

namespace gl
{
	#define PI 3.14159265359

    /**
     * Returns the number of bytes given an OpenGL enum such as GL_SHORT or GL_UNSIGNED_BYTE.
     */
    int sizeOf(GLenum typeEnum);
    
    const char* toString(GLenum glEnum);
    
    /**
     * This will flip the rows a buffer of pixels. This is useful since most images store
     * rows starting at the top, but OpenGL reads starting at the bottom.
     */
    void flipImage(void* buffer, int width, int height, int pixelSize);
}

#endif // __MEDLEAP_GL_UTIL__