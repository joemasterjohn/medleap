#ifndef __MEDLEAP_GL_UTIL__
#define __MEDLEAP_GL_UTIL__

#include "gl/glew.h"
#include <functional>

namespace gl
{

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



	/** f(x) = 0.5(x+1) */
	float linearR(float x);

	/** f(x) = 1-abs(x) */
	float linearC(float x);
	
	/** f(x) = -0.5(x-1) */
	float linearL(float x);

	/** f(x) = 1-0.25(x-1)^2 */
	float quadraticR(float x);

	/** f(x) = 1-x^2 */
	float quadraticC(float x);

	/** f(x) = 1-0.25(x+1)^2 */
	float quadraticL(float x);

	/** f(x) = (a-b)/(c-b), a = 1/(1+exp(-sx)), b = 1/(1+exp(s)), c = 1/(1+exp(-s))  */
	std::function<float(float)> exponentialR(float s);

	/** f(x) = (e^(-sx^2) - e^(-s)) / (1 - e^(-s)) */
	std::function<float(float)> exponentialC(float s);

	/** f(x) = (a-c)/(b-c), a = 1/(1+exp(-sx)), b = 1/(1+exp(s)), c = 1/(1+exp(-s))  */
	std::function<float(float)> exponentialL(float s);
}

#endif // __MEDLEAP_GL_UTIL__