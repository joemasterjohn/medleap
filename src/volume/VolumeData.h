#ifndef __MEDLEAP_VOLUME_DATA__
#define __MEDLEAP_VOLUME_DATA__

#include <GL/glew.h>
#include <vector>
#include "Window.h"

/** Stores stuff **/
class VolumeData
{
public:
    /** Creates an empty volume with memory allocated to fit the specified dimensions */
    VolumeData(int width, int height, int depth, GLenum format, GLenum type);
    
    /** Creates a volume using the memory stored in data; does not copy or allocate new memory */
    VolumeData(char* data, int width, int height, int depth, GLenum format, GLenum type);
    
    /** Stores the image at depth into the provided 2D texture */
    void loadTexture2D(GLuint& texture, int depth);
    
    /** Number of pixels horizontally in each slice */
    unsigned int getWidth();
    
    /** Number of pixels vertically in each slice */
    unsigned int getHeight();
    
    /** Number of slices */
    unsigned int getDepth();
    
    /** Size in bytes of a single slice/image */
    unsigned int getImageSize();
    
    /** Size in bytes of a single pixel */
    unsigned int getPixelSize();
    
    /** Size in bytes of all slices */
    unsigned int getVolumeSize();
    
    /** Windows that store values of interest */
    std::vector<Window>& getWindows();
    
    /** Type for channels in the images */
    GLenum getType();
    
    /** Data is stored in a signed format */
    bool isSigned();

protected:
    char* data;
    int width;
    int height;
    int depth;
    GLenum format;
    GLenum type;
    std::vector<Window> windows;
    
    GLenum internalFormat();
};

#endif // __MEDLEAP_VOLUME_DATA__