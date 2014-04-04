#include "TextRenderer.h"
#include <fstream>
#include <iostream>

using namespace gl;
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

TextRenderer::TextRenderer() : program(0), vbo(0), r(0), g(0), b(0), currentFont(0), dirty(true)
{
}

TextRenderer::~TextRenderer()
{
    for (map<string, Font*>::iterator it = fonts.begin(); it != fonts.end(); it++) {
        delete it->second;
    }
    
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

void TextRenderer::setFont(std::string fontName)
{
    if (fonts.find(fontName) != fonts.end())
        currentFont = fonts[fontName];
}

void TextRenderer::begin(int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    vertices.clear();
    dirty = true;
}

void TextRenderer::addVertex(int x, int y, float u, float v)
{
    vertices.push_back(((float)x / windowWidth - 0.5f) * 2.0f);
    vertices.push_back(((float)y / windowHeight - 0.5f) * 2.0f);
    vertices.push_back(u);
    vertices.push_back(v);
}

void TextRenderer::add(const char* text, int x, int y, Alignment hAlign, Alignment vAlign)
{
    if (hAlign == CENTER) {
        x -= currentFont->measure(text) / 2;
    } else if (hAlign == RIGHT) {
        x -= currentFont->measure(text);
    }
    
    if (vAlign == CENTER) {
        y -= currentFont->glyphHeight / 2;
    } else if (vAlign == TOP) {
        y -= currentFont->glyphHeight;
    }
    
    for (; *text; text++) {
        const char c = *text;
        int glyphWidth = currentFont->glyphWidths[(unsigned)c];
        int glyphHeight = currentFont->glyphHeight;
        
        GLfloat u = GLYPH_STEP * (c % 16);
        GLfloat v = 1.0f - GLYPH_STEP * (c / 16 + 1);
        float uStep = (float)glyphWidth / currentFont->texWidth;
        
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
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    if (dirty) {
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_DYNAMIC_DRAW);
        dirty = false;
    }
    
    // set state
    GLsizei stride = 4 * sizeof(GLfloat);
    int loc = program->getAttribute("vs_position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, 0);
    loc = program->getAttribute("vs_texcoord");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, (GLvoid*)(2 * sizeof(GLfloat)));
    program->enable();
    glUniform3f(program->getUniform("color"), r, g, b);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, currentFont->texture);
    
    // render
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 4);
    
    // reset state
    glDisable(GL_BLEND);
}

int TextRenderer::measureHeight(const char *text)
{
    return currentFont->glyphHeight;
}

int TextRenderer::measureWidth(const char *text)
{
    int length = 0;
    while (*text)
        length += currentFont->glyphWidths[(unsigned)*text++];
    return length;
}

bool TextRenderer::loadFont(std::string fontName)
{
    // create shader program and vertex buffer if first load
    if (!program) {
        program = Program::createFromSrc(vSrc, fSrc);
        glGenBuffers(1, &vbo);
    }
    
    std::string bmpFileName = std::string("fonts/" + fontName + ".bmp");
    std::string metricsFileName = std::string("fonts/" + fontName + ".dat");
    
    Font* font = new Font;
    if (!font->load(bmpFileName.c_str(), metricsFileName.c_str())) {
        delete font;
        return false;
    }
    
    fonts[fontName] = font;
    if (!currentFont)
        currentFont = font;
    
    return font;
}

TextRenderer::Font::Font()
{
}

TextRenderer::Font::~Font()
{
    if (texture)
        glDeleteTextures(1, &texture);
}

int TextRenderer::Font::measure(const char* s)
{
    int width = 0;
    for (; *s; s++)
        width += glyphWidths[(unsigned)*s];
    return width;
}

bool TextRenderer::Font::load(const char* bmpFileName, const char* metricsFileName)
{
    ifstream fin(bmpFileName, ios::in | ios::binary);
    if (!fin) {
        cerr << "File not found: " + std::string(bmpFileName) << endl;
        return false;
    }
    
    // read the byte offset for pixel data start
    fin.seekg(0x0A, ios::beg);
    unsigned short dataStart = 0;
    fin.read((char*)&dataStart, sizeof(unsigned char));
    
    // read the image dimensions
    fin.seekg(0x12, ios::beg);
    fin.read((char*)&texWidth, sizeof(unsigned int));
    fin.read((char*)&texHeight, sizeof(unsigned int));
    
    // read bit planes
    unsigned short planes = 0;
    fin.read((char*)&planes, sizeof(unsigned short));
    if (planes != 1) {
        cerr << "Number of color planes must be 1" << endl;
        return false;
    }
    
    // read bits per pixel
    unsigned short bpp = 0;
    fin.read((char*)&bpp, sizeof(unsigned short));
    if (bpp != 24) {
        cerr << "Image must be 24 bits per pixel (BGR format)" << endl;
        return false;
    }
    
    // read the pixel data
    unsigned size = texWidth * texHeight * 3;
    unsigned char* data = new unsigned char[size];
    fin.seekg(dataStart, ios::beg);
    fin.read((char*)data, sizeof(unsigned char) * size);
    fin.close();
    
    // process into texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    delete[] data;
    
    // glyphs (assuming 16x16 in texture) and monospaced
    ifstream mfile(metricsFileName, ios::in | ios::binary);
    if (mfile) {
        mfile.read((char*)glyphWidths, sizeof(unsigned char)*256);
        mfile.close();
    } else {
        for (int i = 0; i < 256; i++)
            glyphWidths[i] = texWidth / 16;
    }
    glyphHeight = texHeight / 16;
    
    return true;
}