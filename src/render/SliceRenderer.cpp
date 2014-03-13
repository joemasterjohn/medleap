#include "SliceRenderer.h"
#include "math/Transform.h"

using namespace cgl;

// Patient orientation axes with respective labels
// -----------------------------------------------------------------
struct AxisLabel
{
    std::string label;
    cgl::Vec3 vector;
};

static const AxisLabel axes[] = {
    { "L", cgl::Vec3(1,0,0) },  // left
    { "R", cgl::Vec3(-1,0,0) }, // right
    { "P", cgl::Vec3(0,1,0) },  // posterior (back)
    { "A", cgl::Vec3(0,-1,0) }, // anterior (front)
    { "S", cgl::Vec3(0,0,1) },  // superior (head)
    { "I", cgl::Vec3(0,0,-1) }, // inferior (foot)
};
// -----------------------------------------------------------------

SliceRenderer::SliceRenderer() :
    volume(NULL),
    sliceShader(NULL),
    axisShader(NULL),
    sliceTexture(0),
    vao(0),
    vbo(0),
    orientationVBO(0),
    numOrientationVertices(0),
    windowWidth(0),
    windowHeight(0),
    currentSlice(0)
{
}

SliceRenderer::~SliceRenderer()
{
    // TODO: delete resources
}

void SliceRenderer::setVolume(VolumeData* volume)
{
    this->volume = volume;
    
    // set texture to the first slice in the volume
    volume->loadTexture2D(sliceTexture, currentSlice = 0);
    
    // vertex buffer and text labels for the orientation lines
    labels.clear();
    std::vector<GLfloat> vertexData;
    Mat3 patient2image = volume->getPatientBasis().transpose();
    for (int i = 0; i < sizeof(axes) / sizeof(AxisLabel); i++) {
        const AxisLabel& axis = axes[i];
        cgl::Vec3 v = patient2image * axis.vector;
        
        // DICOM image space means +X is right and +Y is down; in OpenGL,
        // +Y is up so negate the y component
        v.y *= -1;
        
        // add the line if its projection on the XY image plane has some length
        if (std::sqrt(v.x * v.x + v.y * v.y) > 0.1) {
            cgl::Vec3 color = (v * 0.5) + 0.5;
            
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
            
            labels.push_back(OrientationLabel(axis.label, cgl::Vec2(v)));
            numOrientationVertices += 2;
        }
    }
    
    glGenBuffers(1, &orientationVBO);
    glBindBuffer(GL_ARRAY_BUFFER, orientationVBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLfloat), &vertexData[0], GL_STATIC_DRAW);
    
    resize(windowWidth, windowHeight);
}

void SliceRenderer::resize(int width, int height)
{
    this->windowWidth = width;
    this->windowHeight = height;
    
    // model matrix will scale to keep the displayed image in proportion to its
    // intended dimensions without changing the input vertices in NDC
    float windowAspect = (float)width / height;
    float sliceAspect = (float)volume->getWidth() / volume->getHeight();
    
    modelMatrix = (sliceAspect <= 1.0f) ?
        cgl::scale(sliceAspect / windowAspect, 1.0f, 1.0f) :
        cgl::scale(1.0f, windowAspect / sliceAspect, 1.0f);
}

void SliceRenderer::init()
{
    // TODO rename these shaders
    sliceShader = Program::create("shaders/slice2D.vert", "shaders/slice2D.frag");
    axisShader = Program::create("shaders/color.vert", "shaders/color.frag");

    // load fonts for text rendering
    text.loadFont("menlo14");
    
    // create a texture to store the current slice
    glGenTextures(1, &sliceTexture);
    
    // create a VAO for 2D rendering
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

void SliceRenderer::drawSlice()
{
    sliceShader->enable();
    
    // set the uniforms
    glUniform1i(sliceShader->getUniform("signed_normalized"), volume->isSigned());
    glUniform1f(sliceShader->getUniform("window_min"), volume->getCurrentWindow().getMinNorm());
    glUniform1f(sliceShader->getUniform("window_multiplier"), 1.0f / volume->getCurrentWindow().getWidthNorm());
    glUniformMatrix4fv(sliceShader->getUniform("model"), 1, false, modelMatrix);
    
    // set state and shader for drawing medical stuff
    GLsizei stride = 4 * sizeof(GLfloat);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    int loc = sliceShader->getAttribute("vs_position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, 0);
    
    loc = sliceShader->getAttribute("vs_texcoord");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, (GLvoid*)(2 * sizeof(GLfloat)));
    
    glBindTexture(GL_TEXTURE_2D, sliceTexture);
    
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    
}

void SliceRenderer::drawOrientationOverlay()
{
    // draw lines
    axisShader->enable();
    float aspect = (float)windowWidth / windowHeight;
    if (aspect >= 1) {
        glUniformMatrix4fv(axisShader->getUniform("modelViewProjection"), 1, false, cgl::scale(1.0f/aspect, 1, 1));
    } else {
        glUniformMatrix4fv(axisShader->getUniform("modelViewProjection"), 1, false, cgl::scale(1, aspect, 1));
    }
    GLsizei stride = 5 * sizeof(GLfloat);
    
    glBindBuffer(GL_ARRAY_BUFFER, orientationVBO);

    int loc = axisShader->getAttribute("vs_position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, 0);
    
    loc = axisShader->getAttribute("vs_color");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, false, stride, (GLvoid*)(2 * sizeof(GLfloat)));
    
    glDrawArrays(GL_LINES, 0, numOrientationVertices);
    
    // draw the text (could be cleaner and more efficient)
    text.setColor(1, 1, 1);
    text.begin(windowWidth, windowHeight);
    for (int i = 0; i < labels.size(); i++) {
        float fx = labels[i].position.x;
        float fy = labels[i].position.y;
        if (aspect >= 1)
            fx *= 1.0f / aspect;
        else
            fy *= aspect;
        
        int x = (int)((fx * 0.5 + 0.5) * windowWidth);
        int y = (int)((fy * 0.5 + 0.5) * windowHeight);
        
        TextRenderer::Alignment hAlign = x < windowWidth / 2 ? TextRenderer::LEFT : TextRenderer::RIGHT;
        TextRenderer::Alignment vAlign = y < windowHeight / 2 ? TextRenderer::BOTTOM : TextRenderer::TOP;
        text.add(labels[i].text.c_str(), x, y, hAlign, vAlign);
    }
    
    // Slice #
    char buf[60];
    sprintf(buf, "slice: %d/%d", (currentSlice+1), volume->getDepth());
    text.add(buf, 0, 0, TextRenderer::LEFT, TextRenderer::BOTTOM);
    
    text.end();
}

void SliceRenderer::draw()
{
    glBindVertexArray(vao);
    drawSlice();
    drawOrientationOverlay();
}

int SliceRenderer::getCurrentSlice()
{
    return currentSlice;
}

void SliceRenderer::setCurrentSlice(int sliceIndex)
{
    volume->loadTexture2D(sliceTexture, currentSlice = sliceIndex);
}