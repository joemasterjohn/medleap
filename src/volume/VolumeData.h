#ifndef __MEDLEAP_VOLUME_DATA__
#define __MEDLEAP_VOLUME_DATA__

#include <GL/glew.h>

/** Stores stuff **/
class VolumeData
{
public:
    VolumeData(unsigned char* data, int width, int height, int depth, GLenum format, GLenum type);
    
    /** Pointer to the data buffer starting at the image at 'depth' */
    unsigned char* imagePointer(int depth);
    
    /** Stores the image at depth into the provided 2D texture */
    void loadTexture2D(GLuint& texture, int depth);
    
//private:
    int width;
    int height;
    int depth;
    unsigned char* data;
    GLenum type;
    GLenum format;
};

#endif // __MEDLEAP_VOLUME_DATA__