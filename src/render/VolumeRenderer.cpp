#include "VolumeRenderer.h"
#include "math/Transform.h"
#include "volume/BoxSlicer.h"

using namespace cgl;
using namespace std;

VolumeRenderer::VolumeRenderer()
{
    dirty = true;
    moving = false;
    proxyVertices = NULL;
    proxyIndices = NULL;
    sceneFramebuffer = NULL;
    sceneTexture = NULL;
    sceneProgram = NULL;
    sceneBuffer = NULL;
}

VolumeRenderer::~VolumeRenderer()
{
    if (proxyVertices)
        delete proxyVertices;
    if (proxyIndices)
        delete proxyIndices;
    if (sceneFramebuffer)
        delete sceneFramebuffer;
    if (sceneTexture)
        delete sceneTexture;
    if (sceneProgram)
        delete sceneProgram;
    if (sceneBuffer)
        delete sceneBuffer;
}

cgl::Camera& VolumeRenderer::getCamera()
{
    return camera;
}

void VolumeRenderer::setVolume(VolumeData* volume)
{
    this->volume = volume;
    volume->loadTexture3D(volumeTexture);
    volume->loadGradientTexture(gradientTexture);
}

void VolumeRenderer::setMoving(bool moving)
{
    this->moving = moving;
}

void VolumeRenderer::markDirty()
{
    dirty = true;
}

void VolumeRenderer::init()
{
    volumeTexture = new Texture(GL_TEXTURE_3D);
    gradientTexture = new Texture(GL_TEXTURE_3D);
    
    camera.setView(cgl::lookAt(0, 0, 2, 0, 0, 0, 0, 1, 0));
    
    lineShader = Program::create("shaders/color.vert", "shaders/color.frag");
    
    sceneProgram = Program::create("shaders/simple_tex.vert", "shaders/simple_tex.frag");
    
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
    
    proxyVertices = new Buffer(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
    proxyIndices = new Buffer(GL_ELEMENT_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
    
    boxShader = Program::create("shaders/vol_mip_1D.vert", "shaders/vol_mip_1D.frag");
    boxShader->enable();
    glUniform1i(boxShader->getUniform("tex_volume"), 0);
    glUniform1i(boxShader->getUniform("tex_gradients"), 1);
    
    
    
    sceneFramebuffer = new Framebuffer;
    sceneTexture = new Texture(GL_TEXTURE_2D);
    sceneTexture->bind();
    sceneTexture->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    sceneTexture->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    sceneTexture->setData2D(GL_RGB, 512, 512, GL_RGB, GL_UNSIGNED_BYTE, 0); // TODO: srgb????
    
    sceneFramebuffer->bind();
    sceneFramebuffer->setColorTarget(0, sceneTexture);
    sceneFramebuffer->unbind();
    
    sceneBuffer = new Buffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    {
        GLfloat vdata[] = {
            -1, -1, 0, 0,
            1, -1, 1, 0,
            1, 1, 1, 1,
            
            -1, -1, 0, 0,
            1, 1, 1, 1,
            -1, 1, 0, 1
        };
        sceneBuffer->bind();
        sceneBuffer->setData(vdata, sizeof(vdata));
    }
}

void VolumeRenderer::resize(int width, int height)
{
    viewport.width = width;
    viewport.height = height;
    camera.setProjection(cgl::perspective(1.0471975512, viewport.aspect(), 0.1f, 100.0f));
}

void VolumeRenderer::updateSlices()
{
    BoxSlicer slicer;
    slicer.slice(volume->getBounds(), camera, moving ? 128 : 512);
    
    proxyIndices->bind();
    proxyIndices->setData(&slicer.getIndices()[0],
                          slicer.getIndices().size() * sizeof(GLushort));
    
    proxyVertices->bind();
    proxyVertices->setData(&slicer.getVertices()[0],
                           slicer.getVertices().size() * sizeof(slicer.getVertices()[0]));
    
    numSliceIndices = slicer.getIndices().size();
}

void VolumeRenderer::draw()
{
    glBindVertexArray(vao);
    

    if (dirty)
    {
        
        // bind framebuffer with full viewport
        sceneFramebuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, sceneTexture->getWidth(), sceneTexture->getHeight());
        
        
        // axes
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            
            lineShader->enable();
            
            cgl::Mat4 mvp = camera.getProjection() * camera.getView();
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
        
        // proxy geometry
        {
            glActiveTexture(GL_TEXTURE1);
            gradientTexture->bind();
            glActiveTexture(GL_TEXTURE0);
            volumeTexture->bind();
            
            updateSlices();
            
            
            boxShader->enable();
            
            cgl::Mat4 mvp = camera.getProjection() * camera.getView();
            glUniformMatrix4fv(boxShader->getUniform("modelViewProjection"), 1, false, mvp);
            
            glUniform3fv(boxShader->getUniform("volumeMin"), 1, volume->getBounds().getMinimum());
            glUniform3fv(boxShader->getUniform("volumeDimensions"), 1, (volume->getBounds().getMaximum() - volume->getBounds().getMinimum()));
            glUniform1i(boxShader->getUniform("signed_normalized"), volume->isSigned());
            
            glUniform1f(boxShader->getUniform("window_min"), volume->getCurrentWindow().getMinNorm());
            glUniform1f(boxShader->getUniform("window_multiplier"), 1.0f / volume->getCurrentWindow().getWidthNorm());
            
            glUniform1f(boxShader->getUniform("opacityCorrection"), moving ? 4.0f : 1.0f);
            glUniform3f(boxShader->getUniform("lightDirection"), camera.getForward().x, camera.getForward().y, camera.getForward().z);
            
            glUniform3f(boxShader->getUniform("minGradient"), volume->getMinGradient().x, volume->getMinGradient().y, volume->getMinGradient().z);
            
            cgl::Vec3 r = volume->getMaxGradient() - volume->getMinGradient();
            glUniform3f(boxShader->getUniform("rangeGradient"), r.x, r.y, r.z);
            
            int loc = boxShader->getAttribute("vs_position");
            glEnableVertexAttribArray(loc);
            glVertexAttribPointer(loc, 3, GL_FLOAT, false, 0, 0);
            
            
            glEnable(GL_BLEND);
            
            // MIP:
            //        glBlendEquation(GL_MAX);
            
            
            // DVR
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            
            
            glEnable(GL_PRIMITIVE_RESTART);
            glPrimitiveRestartIndex(65535);
            glDrawElements(GL_TRIANGLE_FAN, numSliceIndices, GL_UNSIGNED_SHORT, 0);
            glDisable(GL_PRIMITIVE_RESTART);
            
            // MIP:
            glBlendEquation(GL_FUNC_ADD);
            
            // DVR:
            
            
            glDisable(GL_BLEND);
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        std::cout << "rendered a frame" << std::endl;
        dirty = false;
    }
    
    sceneFramebuffer->unbind();
    // render from the texture now
    {
        // use original viewport
        viewport.apply();
        sceneBuffer->bind();
        
        sceneProgram->enable();
        sceneTexture->bind();
        
        GLsizei stride = sizeof(GLfloat) * 4;
        int loc = sceneProgram->getAttribute("vs_position");
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, 0);
        loc = sceneProgram->getAttribute("vs_texcoord");
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, (GLvoid*)(2 * sizeof(GLfloat)));
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
    }
}