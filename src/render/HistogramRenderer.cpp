#include "HistogramRenderer.h"
#include "math/Transform.h"
#include "ui/MainController.h"
#include <string>

// this only applies to CT modality
const char* hounsfield(int value)
{
    if (value <= -900)
        return "AIR";
    
    if (value <= -450 && value >= -550)
        return "LUNG";
    
    if (value <= -50 && value >= -100)
        return "FAT";
    
    if (value <= 5 && value >= -5)
        return "WATER";
    
    if (value <= 20 && value >= 10)
        return "MUSCLE";
    
    if (value <= 30 && value >= 20)
        return "MUSCLE/WHITE MATTER";
    
    if (value <= 37 && value >= 30)
        return "MUSCLE/BLOOD";
    
    if (value <= 40 && value >= 35)
        return "MUSCLE/BLOOD/GREY MATTER";
    
    if (value <= 45 && value >= 40)
        return "LIVER/BLOOD/GREY MATTER";
    
    if (value <= 60 && value >= 45)
        return "LIVER";
    
    if (value <= 300 && value >= 100)
        return "SOFT TISSUE";
    
    if (value >= 700)
        return "BONE";
    
    return "";
}

HistogramRenderer::HistogramRenderer() :
    histo1D(NULL),
    shader(NULL),
    colorShader(NULL)
{
}

HistogramRenderer::~HistogramRenderer()
{
    if (histo1D)
        delete histo1D;
    if (shader)
        delete shader;
    if (colorShader)
        delete colorShader;
}

void HistogramRenderer::init()
{
    histo1D = new cgl::Texture(GL_TEXTURE_2D);
    unsigned char data[] = { 0, 64, 128, 255 };
    histo1D->bind();
    histo1D->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    histo1D->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    histo1D->setData2D(GL_RED, 2, 2, GL_RED, GL_UNSIGNED_BYTE, data);
    
    
    shader = Program::create("shaders/histogram.vert", "shaders/histogram.frag");
    colorShader = Program::create("shaders/histo_line.vert", "shaders/histo_line.frag");
    
    // create a VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // vertex buffer for geometry: contains vertices for
    // 1) the histogram quad (drawn as a texture)
    // 2) the cursor / value marker line
    GLfloat vertexData[] = {
        // start of texture quad vertices
        -1, -1, 0, 0,
        1, -1, 1, 0,
        1,  1, 1, 1,
        -1, -1, 0, 0,
        1,  1, 1, 1,
        -1,  1, 0, 1,
        // start of cursor line vertices
        -1, -1, 0, 0,
        -1, 1, 0, 0
        
    };
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    
    stride = 4 * sizeof(GLfloat);
}

void HistogramRenderer::draw()
{
    // histogram texture
    shader->enable();
    glUniformMatrix4fv(shader->getUniform("model"), 1, false, histoModelMatrix);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    int loc = shader->getAttribute("vs_position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, 0);
    loc = shader->getAttribute("vs_texcoord");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, (GLvoid*)(2 * sizeof(GLfloat)));
    histo1D->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    drawCursorValue();
}

void HistogramRenderer::drawCursorValue()
{
    // cursor line
    colorShader->enable();
    glUniform1f(colorShader->getUniform("offset"), cursorShaderOffset);
    int loc = colorShader->getAttribute("vs_position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, 0);
    glDrawArrays(GL_LINES, 6, 2);
    
    // cursor histogram value
    TextRenderer& text = MainController::getInstance().getText();
    text.setColor(1, 1, 0);
    text.begin(viewport.width, viewport.height);
    char buf[120];
    
    if (volume->getModality() == VolumeData::CT)
        sprintf(buf, "%.1f %s", cursorValue, hounsfield(cursorValue));
    else
        sprintf(buf, "%.1f", cursorValue);
    
    if (cursorX > viewport.width / 2)
        text.add(buf, cursorX - 5, cursorY, TextRenderer::RIGHT);
    else
        text.add(buf, cursorX + 5, cursorY, TextRenderer::LEFT);
    text.end();
}

void HistogramRenderer::resize(int width, int height)
{
}

void HistogramRenderer::setVolume(VolumeData* volume)
{
    this->volume = volume;
}

void HistogramRenderer::setHistogram(Histogram* histogram)
{
    this->histogram = histogram;
    int drawWidth = histogram->getNumBins();
    int drawHeight = 256;
    
    unsigned char pixels[drawWidth * drawHeight];
    std::fill(pixels, pixels + drawWidth * drawHeight, 0);
    
    for (int bin = 0; bin < histogram->getNumBins(); bin++) {
        int size = histogram->getSize(bin);
        
        // linear y scale:
//        double sizeNorm = (double)size / histogram->getMaxFrequency();
        // logarithmic y scale:
        double sizeNorm = (double)std::log(size)/std::log(histogram->getMaxFrequency());
        
        int binHeight = (int)(sizeNorm * drawHeight);
        
        for (int j = 0; j < binHeight; j++) {
            pixels[bin + j * drawWidth] = 255;
        }
    }
    
    histo1D->bind();
    histo1D->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    histo1D->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    histo1D->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    histo1D->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    histo1D->setData2D(GL_RED, drawWidth, drawHeight, GL_RED, GL_UNSIGNED_BYTE, pixels);
}

void HistogramRenderer::setCursor(int x, int y)
{
    if (!viewport.contains(x, y)) {
        return;
    }
    cursorX = x;
    cursorY = y;
    double px = (double)x / viewport.width;
    cursorValue = px * (histogram->getMax() - histogram->getMin()) + histogram->getMin();
    cursorShaderOffset = px * 2.0;
}