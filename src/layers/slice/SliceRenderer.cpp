#include "SliceRenderer.h"
#include "gl/math/Transform.h"
#include "main/MainController.h"

using namespace gl;

// Patient orientation axes with respective labels
// -----------------------------------------------------------------
struct AxisLabel
{
    std::string label;
    Vec3 vector;
};

static const AxisLabel axes[] = {
    { "L", Vec3(1,0,0) },  // left
    { "R", Vec3(-1,0,0) }, // right
    { "P", Vec3(0,1,0) },  // posterior (back)
    { "A", Vec3(0,-1,0) }, // anterior (front)
    { "S", Vec3(0,0,1) },  // superior (head)
    { "I", Vec3(0,0,-1) }, // inferior (foot)
};
// -----------------------------------------------------------------

SliceRenderer::SliceRenderer() :
    volume(NULL),
    numOrientationVertices(0),
    currentSlice(0)
{
}

SliceRenderer::~SliceRenderer()
{
}

void SliceRenderer::setCLUTTexture(gl::Texture& texture)
{
    this->clutTexture = texture;
}

void SliceRenderer::updateTexture()
{
    sliceTexture.bind();
    sliceTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    sliceTexture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
	GLenum internalFormat;
	switch (volume->getType()) {
		case GL_BYTE: internalFormat = GL_R8_SNORM; break;
		case GL_SHORT: internalFormat = GL_R16_SNORM; break;
		default: internalFormat = GL_RED; break;
	}

    sliceTexture.setData2D(0,
                            internalFormat,
                            volume->getWidth(),
                            volume->getHeight(),
                            volume->getFormat(),
                            volume->getType(),
                            volume->getData() + currentSlice * volume->getSliceSizeBytes());
}

void SliceRenderer::setVolume(VolumeData* volume)
{
    this->volume = volume;
    
    // set texture to the first slice in the volume
    updateTexture();
    
    // vertex buffer and text labels for the orientation lines
    labels.clear();
    std::vector<GLfloat> vertexData;
    Mat3 patient2image = volume->getPatientBasis().transpose();
    for (int i = 0; i < sizeof(axes) / sizeof(AxisLabel); i++) {
        const AxisLabel& axis = axes[i];
        Vec3 v = patient2image * axis.vector;
        
        // DICOM image space means +X is right and +Y is down; in OpenGL,
        // +Y is up so negate the y component
        v.y *= -1;
        
        // add the line if its projection on the XY image plane has some length
        if (std::sqrt(v.x * v.x + v.y * v.y) > 0.1) {
            Vec3 color = (v * 0.5) + 0.5;
            
            vertexData.push_back(0);
            vertexData.push_back(0);
            vertexData.push_back(color.x);
            vertexData.push_back(color.y);
            vertexData.push_back(color.z);
            
            vertexData.push_back(v.x);
            vertexData.push_back(v.y);
            vertexData.push_back(color.x);
            vertexData.push_back(color.y);
            vertexData.push_back(color.z);
            
            labels.push_back(OrientationLabel(axis.label, Vec2(v)));
            numOrientationVertices += 2;
        }
    }
    
    axisVBO = Buffer::genVertexBuffer();
    axisVBO.bind();
    axisVBO.setData(&vertexData[0], vertexData.size() * sizeof(GLfloat));
    
    resize(viewport.width, viewport.height);
}

void SliceRenderer::resize(int width, int height)
{
    // model matrix will scale to keep the displayed image in proportion to its
    // intended dimensions without changing the input vertices in NDC
    float windowAspect = (float)width / height;
    float sliceAspect = (float)volume->getWidth() / volume->getHeight();
    
    modelMatrix = (sliceAspect <= 1.0f) ?
        scale(sliceAspect / windowAspect, 1.0f, 1.0f) :
        scale(1.0f, windowAspect / sliceAspect, 1.0f);
}

void SliceRenderer::init()
{
    sliceShader = Program::create("shaders/slice_clut.vert", "shaders/slice_clut.frag");
    axisShader = Program::create("shaders/color.vert", "shaders/color.frag");
    
    sliceShader.enable();
    glUniform1i(sliceShader.getUniform("tex_slice"), 0);
    glUniform1i(sliceShader.getUniform("tex_clut"), 1);
    
    // create a texture to store the current slice
	sliceTexture.generate(GL_TEXTURE_2D);
    
    // vertex buffer for the slice image geometry
    GLfloat vertexData[] = {
        -1, -1, 0, 0,
         1, -1, 1, 0,
         1,  1, 1, 1,
        -1, -1, 0, 0,
         1,  1, 1, 1,
        -1,  1, 0, 1
    };
    
    sliceVBO = Buffer::genVertexBuffer();
    sliceVBO.bind();
    sliceVBO.setData(vertexData, sizeof(vertexData));
}

void SliceRenderer::drawSlice()
{
    sliceShader.enable();
    
    glActiveTexture(GL_TEXTURE1);
    clutTexture.bind();
    glActiveTexture(GL_TEXTURE0);
    sliceTexture.bind();
    
    // set the uniforms
    glUniform1i(sliceShader.getUniform("signed_normalized"), volume->isSigned());
    glUniform1f(sliceShader.getUniform("window_min"), volume->getCurrentWindow().getMinNorm());
    glUniform1f(sliceShader.getUniform("window_multiplier"), 1.0f / volume->getCurrentWindow().getWidthNorm());
    glUniformMatrix4fv(sliceShader.getUniform("model"), 1, false, modelMatrix);
    
    // set state and shader for drawing medical stuff
    GLsizei stride = 4 * sizeof(GLfloat);
    sliceVBO.bind();
    
    int loc = sliceShader.getAttribute("vs_position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, 0);
    
    loc = sliceShader.getAttribute("vs_texcoord");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, (GLvoid*)(2 * sizeof(GLfloat)));
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SliceRenderer::drawOrientationOverlay()
{
    TextRenderer& text = MainController::getInstance().getText();
    
    // draw lines
    axisShader.enable();
    float aspect = viewport.aspect();
    if (aspect >= 1) {
        glUniformMatrix4fv(axisShader.getUniform("modelViewProjection"), 1, false, scale(1.0f/aspect, 1, 1));
    } else {
        glUniformMatrix4fv(axisShader.getUniform("modelViewProjection"), 1, false, scale(1, aspect, 1));
    }
    GLsizei stride = 5 * sizeof(GLfloat);

    axisVBO.bind();

    int loc = axisShader.getAttribute("vs_position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, 0);
    
    loc = axisShader.getAttribute("vs_color");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, false, stride, (GLvoid*)(2 * sizeof(GLfloat)));
    
    glDrawArrays(GL_LINES, 0, numOrientationVertices);
    
    // draw the text (could be cleaner and more efficient)
    text.setColor(1, 1, 1);
    text.begin(viewport.width, viewport.height);
    for (int i = 0; i < labels.size(); i++) {
        float fx = labels[i].position.x;
        float fy = labels[i].position.y;
        if (aspect >= 1)
            fx *= 1.0f / aspect;
        else
            fy *= aspect;
        
        int x = (int)((fx * 0.5 + 0.5) * viewport.width);
        int y = (int)((fy * 0.5 + 0.5) * viewport.height);
        
        TextRenderer::Alignment hAlign = x < viewport.width / 2 ? TextRenderer::LEFT : TextRenderer::RIGHT;
        TextRenderer::Alignment vAlign = y < viewport.height / 2 ? TextRenderer::BOTTOM : TextRenderer::TOP;
        text.add(labels[i].text, x, y, hAlign, vAlign);
    }
    
    text.end();
}

void SliceRenderer::draw()
{
    drawSlice();
    drawOrientationOverlay();
}

int SliceRenderer::getCurrentSlice()
{
    return currentSlice;
}

void SliceRenderer::setCurrentSlice(int sliceIndex)
{
    currentSlice = sliceIndex;
    updateTexture();
}