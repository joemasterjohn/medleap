#include "Renderer3D.h"
#include "math/Transform.h"
#include "volume/BoxSlicer.h"

using namespace cgl;
using namespace std;

Renderer3D::Renderer3D()
{
}

Renderer3D::~Renderer3D()
{
}

cgl::Camera& Renderer3D::getCamera()
{
    return camera;
}

void Renderer3D::setVolume(DCMImageSeries* volume)
{
    this->volume = volume;
    volume->loadTexture3D(volumeTexture);
}

void Renderer3D::init()
{
    
    volumeTexture = new Texture(GL_TEXTURE_3D);
    
    camera.setView(cgl::lookAt(0, 0, 2, 0, 0, 0, 0, 1, 0));
    
    lineShader = Program::create("shaders/color.vert", "shaders/color.frag");
    
    GLfloat vertexData[] = {
        0, 0, 0, 1, 0, 0,
        1, 0, 0, 1, 0, 0,
        
        0, 0, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 0,
        
        0, 0, 0, 0, 0, 1,
        0, 0, 1, 0, 0, 1
    };
    numGridVerts = 6;
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    
    

    
    
    
    
    
    
    boxShader = Program::create("shaders/test.vert", "shaders/test.frag");
    
}

void Renderer3D::resize(int width, int height)
{
    viewport.width = width;
    viewport.height = height;
    camera.setProjection(cgl::perspective(1.0471975512, 1, 0.1f, 100.0f));
}

void Renderer3D::updateSlices()
{
    BoxSlicer slicer;
    slicer.slice(volume->getBounds(), camera, 128);
    
    glGenBuffers(1, &boxIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 slicer.getIndices().size() * sizeof(GLushort),
                 &slicer.getIndices()[0],
                 GL_STATIC_DRAW);
    
    glGenBuffers(1, &boxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 slicer.getVertices().size() * sizeof(slicer.getVertices()[0]),
                 &slicer.getVertices()[0],
                 GL_STATIC_DRAW);
    
    numSliceIndices = slicer.getIndices().size();
}

void Renderer3D::draw()
{
    glBindVertexArray(vao);
    
    // axes
    {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        lineShader->enable();
        
        cgl::Mat4 mvp = camera.getProjection() * camera.getView() * model;
        glUniformMatrix4fv(lineShader->getUniform("modelViewProjection"), 1, false, mvp);
        
        GLsizei stride = sizeof(GLfloat) * 6;
        
        int loc = lineShader->getAttribute("vs_position");
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 3, GL_FLOAT, false, stride, 0);
        
        loc = lineShader->getAttribute("vs_color");
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 3, GL_FLOAT, false, stride, (GLvoid*)(3 * sizeof(GLfloat)));
        
        glDrawArrays(GL_LINES, 0, numGridVerts);
    }
    
    // box
    {
        volumeTexture->bind();
        updateSlices();

        glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxIBO);
        
        boxShader->enable();
        
        cgl::Mat4 mvp = camera.getProjection() * camera.getView() * model;
        glUniformMatrix4fv(boxShader->getUniform("modelViewProjection"), 1, false, mvp);
        
        glUniform3fv(boxShader->getUniform("volumeMin"), 1, volume->getBounds().getMinimum());
        glUniform3fv(boxShader->getUniform("volumeDimensions"), 1, (volume->getBounds().getMaximum() - volume->getBounds().getMinimum()));
        glUniform1i(boxShader->getUniform("signed_normalized"), volume->isSigned());
        
        glUniform1f(boxShader->getUniform("window_min"), volume->getCurrentWindow().getMinNorm());
        glUniform1f(boxShader->getUniform("window_multiplier"), 1.0f / volume->getCurrentWindow().getWidthNorm());
        

        int loc = boxShader->getAttribute("vs_position");
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 3, GL_FLOAT, false, 0, 0);
        
        glEnable(GL_BLEND);
//        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_MAX);
//        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(65535);
        glDrawElements(GL_TRIANGLE_FAN, numSliceIndices, GL_UNSIGNED_SHORT, 0);
        glDisable(GL_PRIMITIVE_RESTART);
//        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBlendEquation(GL_FUNC_ADD);

        glDisable(GL_BLEND);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
}