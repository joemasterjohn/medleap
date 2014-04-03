#include "Util.h"
#include <cstring>

int gl::sizeOf(GLenum typeEnum)
{
    switch (typeEnum)
    {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
            return sizeof(GLbyte);
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
            return sizeof(GLshort);
        case GL_INT:
        case GL_UNSIGNED_INT:
            return sizeof(GLint);
        case GL_FLOAT:
            return sizeof(GLfloat);
        case GL_DOUBLE:
            return sizeof(GLdouble);
        default:
            return 0;
    }
}

const char* gl::toString(GLenum glEnum)
{
    switch (glEnum)
    {
        case GL_UNSIGNED_BYTE:
            return "GL_UNSIGNED_BYTE";
        case GL_BYTE:
            return "GL_BYTE";
        case GL_UNSIGNED_SHORT:
            return "GL_UNSIGNED_SHORT";
        case GL_SHORT:
            return "GL_SHORT";
        default:
            return "UNKNOWN";
    }
}

void gl::flipImage(void* buffer, int width, int height, int pixelSize)
{
    char* buf = (char*)buffer;
    int rowSize = pixelSize * width;
    char temp[rowSize];
    
    for (int i = 0; i < height/2; i++) {
        memcpy(temp, buf + i * rowSize, rowSize);
        memcpy(buf + i * rowSize, buf + (height - 1 - i) * rowSize, rowSize);
        memcpy(buf + (height - 1 - i) * rowSize, temp, rowSize);
    }
}