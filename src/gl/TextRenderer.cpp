#include "TextRenderer.h"
#include <fstream>

using namespace std;

static const char* vSrc = "\
#version 150\n\
in  vec4 vs_position;\n\
in  vec2 vs_texcoord;\n\
out vec2 fs_texcoord;\n\
void main()\n\
{\n\
    gl_Position = vs_position;\n\
    fs_texcoord = vs_texcoord;\n\
}";

static const char* fSrc = "\
#version 150\n\
uniform vec3 color;\n\
uniform sampler2D texSampler;\n\
in vec2 fs_texcoord;\n\
out vec4 display_color;\n\
void main()\n\
{\n\
    float intensity = texture(texSampler, fs_texcoord).r;\n\
    display_color = vec4(color, intensity);\n\
}";

// texture coordinate stepping based on a 16x16 grid of glyphs
static const float GLYPH_STEP = 0.0625f;

TextRenderer::TextRenderer() : fontTexture(0), program(0), r(0), g(0), b(0)
{
}

TextRenderer::~TextRenderer()
{
    if (fontTexture)
        glDeleteTextures(1, &fontTexture);
    if (program)
        delete program;
    if (vbo)
        glDeleteBuffers(1, &vbo);
}

void TextRenderer::setColor(float r, float g, float b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

void TextRenderer::begin(int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    vertices.clear();
}

void TextRenderer::addVertex(int x, int y, float u, float v)
{
    vertices.push_back(((float)x / windowWidth - 0.5f) * 2.0f);
    vertices.push_back(((float)y / windowHeight - 0.5f) * 2.0f);
    vertices.push_back(u);
    vertices.push_back(v);
}

void TextRenderer::draw(const char* text, int x, int y)
{
    for (; *text; text++) {
        const char c = *text;
        int glyphWidth = glyphWidths[(unsigned)c];
        
        GLfloat u = GLYPH_STEP * (c % 16);
        GLfloat v = 1.0f - GLYPH_STEP * (c / 16 + 1);
        float uStep = (float)glyphWidth / textureWidth;
        
        addVertex(x, y, u, v);
        addVertex(x + glyphWidth, y, u + uStep, v);
        addVertex(x + glyphWidth, y + glyphHeight, u + uStep, v + GLYPH_STEP);

        addVertex(x, y, u, v);
        addVertex(x + glyphWidth, y + glyphHeight, u + uStep, v + GLYPH_STEP);
        addVertex(x, y + glyphHeight, u, v + GLYPH_STEP);
        
        x += glyphWidth;
    }
}

void TextRenderer::end()
{
    if (vertices.empty())
        return;
    
    GLsizei stride = 4 * sizeof(GLfloat);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_DYNAMIC_DRAW);
    
    int loc = program->getAttribute("vs_position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, 0);
    
    loc = program->getAttribute("vs_texcoord");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, (GLvoid*)(2 * sizeof(GLfloat)));
    
    
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    program->enable();
    glUniform3f(program->getUniform("color"), r, g, b);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 4);
}

std::string TextRenderer::load(const char* bmpFileName, const char* metricsFileName)
{
    unsigned char* data;
    
    // create shader program and vertex buffer
    if (!program) {
        program = Program::createFromSrc(vSrc, fSrc);
        glGenBuffers(1, &vbo);
    }
    
    // open file
    ifstream fin(bmpFileName, ios::in | ios::binary);
    if (!fin)
        return "File not found: " + std::string(bmpFileName);
    
    // read the byte offset for pixel data start
    fin.seekg(0x0A, ios::beg);
    unsigned short dataStart = 0;
    fin.read((char*)&dataStart, sizeof(unsigned char));
    
    // read the image dimensions
    fin.seekg(0x12, ios::beg);
    fin.read((char*)&textureWidth, sizeof(unsigned int));
    fin.read((char*)&textureHeight, sizeof(unsigned int));
    
    unsigned short planes = 0;
    fin.read((char*)&planes, sizeof(unsigned short));
    if (planes != 1)
        return "Number of color planes must be 1";
    
    unsigned short bpp = 0;
    fin.read((char*)&bpp, sizeof(unsigned short));
    if (bpp != 24)
        return "Image must be 24 bits per pixel (BGR format)";
    
    // read the pixel data
    unsigned size = textureWidth * textureHeight * 3;
    data = new unsigned char[size];
    fin.seekg(dataStart, ios::beg);
    fin.read((char*)data, sizeof(unsigned char) * size);
    fin.close();
    
    // process into texture
    if (!fontTexture)
        glGenTextures(1, &fontTexture);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    delete[] data;
    
    // glyphs (assuming 16x16 in texture) and monospaced
    if (metricsFileName) {
        ifstream mfile(metricsFileName, ios::in | ios::binary);
        if (!mfile)
            return "Font metrics file not found: " + std::string(metricsFileName);
        mfile.read((char*)glyphWidths, sizeof(unsigned char)*256);
        mfile.close();
    } else {
        for (int i = 0; i < 256; i++)
            glyphWidths[i] = textureWidth / 16;
    }
    glyphHeight = textureHeight / 16;
    
    return std::string();
}