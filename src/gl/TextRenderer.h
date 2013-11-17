#ifndef __MEDLEAP_TEXT_RENDERER__
#define __MEDLEAP_TEXT_RENDERER__

#include "GL/glew.h"
#include "program.h"
#include <string>
#include <vector>

class TextRenderer
{
public:
    TextRenderer();
    ~TextRenderer();
    std::string load(const char* bmpfile, const char* datfile);
    
    void setColor(float r, float g, float b);
    void begin(int width, int height);
    void draw(const char* text, int x, int y);
    void end();
  
private:    
    GLuint fontTexture;
    Program* program;
    GLuint vbo;
    float r, g, b;
    unsigned char glyphWidths[256];
    unsigned char glyphHeight;
    std::vector<GLfloat> vertices;
    int windowWidth, windowHeight;
    int textureWidth, textureHeight;
    
    void addVertex(int x, int y, float u, float v);
};

#endif // __MEDLEAP_TEXT_RENDERER__