#include "HistogramRenderer.h"
#include "math/Transform.h"

HistogramRenderer::HistogramRenderer() :
    histo1D(NULL),
    shader(NULL)
{
}

HistogramRenderer::~HistogramRenderer()
{
    if (histo1D)
        delete histo1D;
    if (shader)
        delete shader;
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
    
    // create a VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // vertex buffer for the slice image geometry
    GLfloat vertexData[] = {
        -1, -1, 0, 0,
        1, -1, 1, 0,
        1,  1, 1, 1,
        -1, -1, 0, 0,
        1,  1, 1, 1,
        -1,  1, 0, 1
    };
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
}

void HistogramRenderer::draw()
{
    shader->enable();
    
    // set the uniforms
    glUniformMatrix4fv(shader->getUniform("model"), 1, false, histoModelMatrix);
    
    // set state and shader for drawing medical stuff
    GLsizei stride = 4 * sizeof(GLfloat);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    int loc = shader->getAttribute("vs_position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, 0);
    
    loc = shader->getAttribute("vs_texcoord");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, (GLvoid*)(2 * sizeof(GLfloat)));
    
    histo1D->bind();
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void HistogramRenderer::resize(int width, int height)
{
}

void HistogramRenderer::setVolume(VolumeData* volume)
{
}

void HistogramRenderer::setHistogram(Histogram* histogram)
{
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