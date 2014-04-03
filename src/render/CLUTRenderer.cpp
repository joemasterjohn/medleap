#include "CLUTRenderer.h"

using namespace gl;

CLUTRenderer::CLUTRenderer()
{
    clut = NULL;
    clutTexture = NULL;
}

CLUTRenderer::~CLUTRenderer()
{
}

void CLUTRenderer::setCLUT(CLUT* clut)
{
    this->clut = clut;
    if (clutTexture)
        clut->saveTexture(clutTexture);
}

gl::Texture* CLUTRenderer::getTexture()
{
    return clutTexture;
}

void CLUTRenderer::init()
{
    clutTexture = new Texture(GL_TEXTURE_1D);
    clut->saveTexture(clutTexture);
    
    quadShader = Program::create("shaders/texture_1D.vert", "shaders/texture_1D.frag");
    
    GLfloat quadVerts[] = {
        -1, -1, 0,
         1, -1, 1,
         1,  1, 1,
        -1, -1, 0,
         1,  1, 1,
        -1,  1, 0
    };
    
    stride = 3 * sizeof(GLfloat);
    
    quadVBO = new gl::Buffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    quadVBO->bind();
    quadVBO->setData(quadVerts, sizeof(quadVerts));
}

void CLUTRenderer::draw()
{
    quadShader->enable();
    quadVBO->bind();
    clutTexture->bind();
    
    int loc = quadShader->getAttribute("vs_position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, 0);
    
    loc = quadShader->getAttribute("vs_texcoord");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, (GLvoid*)(2 * sizeof(GLfloat)));
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void CLUTRenderer::resize(int width, int height)
{
}