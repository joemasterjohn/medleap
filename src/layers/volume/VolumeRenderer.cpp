#include "VolumeRenderer.h"
#include "math/Transform.h"
#include "BoxSlicer.h"

using namespace gl;
using namespace std;
using namespace glmath;

VolumeRenderer::VolumeRenderer()
{
    dirty = true;
    proxyVertices = NULL;
    proxyIndices = NULL;
    lowResFBO = NULL;
    fullResFBO = NULL;
    lowResFBO = NULL;
    fullResTexture = NULL;
    sceneProgram = NULL;
    sceneBuffer = NULL;
    numSamples = 256;
    opacityScale = 1.0f;
    renderMode = VR;
    shading = true;
    clutTexture = NULL;
    drawnHighRes = false;
}

VolumeRenderer::~VolumeRenderer()
{
    if (proxyVertices)
        delete proxyVertices;
    if (proxyIndices)
        delete proxyIndices;
    if (lowResFBO)
        delete lowResFBO;
    if (fullResFBO)
        delete fullResFBO;
    if (lowResTexture)
        delete lowResTexture;
    if (fullResTexture)
        delete fullResTexture;
    if (sceneProgram)
        delete sceneProgram;
    if (sceneBuffer)
        delete sceneBuffer;
}

Camera& VolumeRenderer::getCamera()
{
    return camera;
}

void VolumeRenderer::setVolume(VolumeData* volume)
{
    this->volume = volume;
    volume->loadTexture3D(volumeTexture);
    volume->loadGradientTexture(gradientTexture);
}

void VolumeRenderer::markDirty()
{
    dirty = true;
}

void VolumeRenderer::setMode(VolumeRenderer::RenderMode mode)
{
    this->renderMode = mode;
    markDirty();
}

void VolumeRenderer::cycleMode()
{
    renderMode = (VolumeRenderer::RenderMode)((renderMode + 1) % VolumeRenderer::NUM_OF_MODES);
    markDirty();
}

VolumeRenderer::RenderMode VolumeRenderer::getMode()
{
    return renderMode;
}

int VolumeRenderer::getNumSamples()
{
    return numSamples;
}

void VolumeRenderer::toggleShading()
{
    shading = !shading;
    markDirty();
}

bool VolumeRenderer::useShading()
{
    return shading;
}

void VolumeRenderer::setCLUTTexture(Texture* texture)
{
    this->clutTexture = texture;
    markDirty();
}

void VolumeRenderer::init()
{
    volumeTexture = new Texture(GL_TEXTURE_3D);
    gradientTexture = new Texture(GL_TEXTURE_3D);

    camera.setView(lookAt(0, 0, 2, 0, 0, 0, 0, 1, 0));
    
    lineShader = Program::create("shaders/color.vert", "shaders/color.frag");
    
    sceneProgram = Program::create("shaders/texture_2D.vert", "shaders/texture_2D.frag");
    
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
    
    boxShader = Program::create("shaders/volume_clut.vert", "shaders/volume_clut.frag");
    boxShader->enable();
    glUniform1i(boxShader->getUniform("tex_volume"), 0);
    glUniform1i(boxShader->getUniform("tex_gradients"), 1);
    glUniform1i(boxShader->getUniform("tex_clut"), 2);

    
    lowResFBO = new Framebuffer;
    lowResTexture = new Texture(GL_TEXTURE_2D);
    lowResTexture->bind();
    lowResTexture->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    lowResTexture->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	lowResTexture->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	lowResTexture->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    lowResTexture->setData2D(GL_RGB, 512, 512, GL_RGB, GL_UNSIGNED_BYTE, 0);
    lowResFBO->bind();
    lowResFBO->setColorTarget(0, lowResTexture);
    lowResFBO->unbind();
    
    fullResFBO = new Framebuffer;
    fullResTexture = new Texture(GL_TEXTURE_2D);
    fullResTexture->bind();
    fullResTexture->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    fullResTexture->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	fullResTexture->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	fullResTexture->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    fullResTexture->setData2D(GL_RGB, viewport.width, viewport.height, GL_RGB, GL_UNSIGNED_BYTE, 0);
    fullResFBO->bind();
    fullResFBO->setColorTarget(0, fullResTexture);
    fullResFBO->unbind();
    
    sceneBuffer = new Buffer(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
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

float VolumeRenderer::getOpacityScale()
{
    return opacityScale;
}

void VolumeRenderer::setOpacityScale(float scale)
{
    this->opacityScale = min(max(0.0f, scale), 1.0f);
    markDirty();
}

void VolumeRenderer::resize(int width, int height)
{
    lowResTexture->bind();
    lowResTexture->setData2D(GL_RGB, viewport.width/2, viewport.height/2, GL_RGB, GL_UNSIGNED_BYTE, 0);
    fullResTexture->bind();
    fullResTexture->setData2D(GL_RGB, viewport.width, viewport.height, GL_RGB, GL_UNSIGNED_BYTE, 0);
    camera.setProjection(perspective(1.047197f, viewport.aspect(), 0.1f, 100.0f));
    markDirty();
}

void VolumeRenderer::updateSlices(int numSlices)
{
    BoxSlicer slicer;
    slicer.slice(volume->getBounds(), camera, numSlices);
    
    proxyIndices->bind();
    proxyIndices->setData(&slicer.getIndices()[0],
                          slicer.getIndices().size() * sizeof(GLushort));
    
    proxyVertices->bind();
    proxyVertices->setData(&slicer.getVertices()[0],
                           slicer.getVertices().size() * sizeof(slicer.getVertices()[0]));
    
    numSliceIndices = static_cast<int>(slicer.getIndices().size());
}

void VolumeRenderer::draw(int numSlices)
{
    // proxy geometry
    glActiveTexture(GL_TEXTURE2);
    clutTexture->bind();
    glActiveTexture(GL_TEXTURE1);
    gradientTexture->bind();
    glActiveTexture(GL_TEXTURE0);
    volumeTexture->bind();
    
    updateSlices(numSlices);
    
    
    boxShader->enable();
    
    Mat4 mvp = camera.getProjection() * camera.getView();
    glUniformMatrix4fv(boxShader->getUniform("modelViewProjection"), 1, false, mvp);
    
    glUniform3fv(boxShader->getUniform("volumeMin"), 1, volume->getBounds().getMinimum());
    glUniform3fv(boxShader->getUniform("volumeDimensions"), 1, (volume->getBounds().getMaximum() - volume->getBounds().getMinimum()));
    glUniform1i(boxShader->getUniform("signed_normalized"), volume->isSigned());
    glUniform1i(boxShader->getUniform("use_shading"), (renderMode != MIP && shading));
    glUniform1f(boxShader->getUniform("window_min"), volume->getCurrentWindow().getMinNorm());
    glUniform1f(boxShader->getUniform("window_multiplier"), 1.0f / volume->getCurrentWindow().getWidthNorm());
    
    glUniform1f(boxShader->getUniform("opacity_correction"), (float)numSamples / numSlices);
    glUniform3f(boxShader->getUniform("lightDirection"), -camera.getForward().x, -camera.getForward().y, -camera.getForward().z);
    
    glUniform3f(boxShader->getUniform("minGradient"), volume->getMinGradient().x, volume->getMinGradient().y, volume->getMinGradient().z);
    glUniform1i(boxShader->getUniform("use_isosurface"), renderMode == ISOSURFACE ? 1 : 0);
    glUniform1f(boxShader->getUniform("opacity_scale"), opacityScale);
    Vec3 r = volume->getMaxGradient() - volume->getMinGradient();
    glUniform3f(boxShader->getUniform("rangeGradient"), r.x, r.y, r.z);
    
    int loc = boxShader->getAttribute("vs_position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, false, 0, 0);
    
    
    glEnable(GL_BLEND);
    
    switch (renderMode)
    {
        case MIP:
            glBlendEquation(GL_MAX);
            glBlendFunc(GL_ONE, GL_ONE);
            break;
        case VR:
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case ISOSURFACE:
            glDisable(GL_BLEND);
            glUniform1f(boxShader->getUniform("isoValue"), volume->getCurrentWindow().getCenterNorm());
            break;
        default:
            break;
    }
    
    
    
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(65535);
    glDrawElements(GL_TRIANGLE_FAN, numSliceIndices, GL_UNSIGNED_SHORT, 0);
    glDisable(GL_PRIMITIVE_RESTART);
    
    
    
    // MIP:
    // DVR:
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    
}

void VolumeRenderer::draw()
{
    glBindVertexArray(vao);
 
    static int cleanFrames = 0;
    
    // draw to texture
    static gl::Texture* currentTexture = NULL;
    if (dirty) {
        lowResFBO->bind();
        glViewport(0, 0, lowResTexture->getWidth(), lowResTexture->getHeight());
        glClear(GL_COLOR_BUFFER_BIT);
        draw(numSamples);
        lowResFBO->unbind();
        dirty = false;
        drawnHighRes = false;
        cleanFrames = 1;
        currentTexture = lowResTexture;
    } else if (!drawnHighRes && cleanFrames++ > 30) {
        fullResFBO->bind();
        glViewport(0, 0, fullResTexture->getWidth(), fullResTexture->getHeight());
        glClear(GL_COLOR_BUFFER_BIT);
        draw(numSamples * 8);
        fullResFBO->unbind();
        drawnHighRes = true;
        currentTexture = fullResTexture;
    }

    
    // draw from texture to screen
    viewport.apply();
    sceneBuffer->bind();
    sceneProgram->enable();
    currentTexture->bind();
    GLsizei stride = sizeof(GLfloat) * 4;
    int loc = sceneProgram->getAttribute("vs_position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, 0);
    loc = sceneProgram->getAttribute("vs_texcoord");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, (GLvoid*)(2 * sizeof(GLfloat)));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    
    
    
    
    
//    // axes
//    {
//        glBindBuffer(GL_ARRAY_BUFFER, vbo);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//        
//        lineShader->enable();
//        
//        Mat4 mvp = camera.getProjection() * camera.getView();
//        glUniformMatrix4fv(lineShader->getUniform("modelViewProjection"), 1, false, mvp);
//        
//        GLsizei stride = sizeof(GLfloat) * 6;
//        
//        int loc = lineShader->getAttribute("vs_position");
//        glEnableVertexAttribArray(loc);
//        glVertexAttribPointer(loc, 3, GL_FLOAT, false, stride, 0);
//        
//        loc = lineShader->getAttribute("vs_color");
//        glEnableVertexAttribArray(loc);
//        glVertexAttribPointer(loc, 3, GL_FLOAT, false, stride, (GLvoid*)(3 * sizeof(GLfloat)));
//        
//        glDrawArrays(GL_LINES, 0, numGridVerts);
//    }
}