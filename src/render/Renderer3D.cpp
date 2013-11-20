#include "Renderer3D.h"
#include "math/Transform.h"

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

void Renderer3D::init()
{
    
    camera.setView(cgl::lookAt(5, 5, 5, 0, 0, 0, 0, 1, 0));
    
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
    
    
    GLsizei stride = sizeof(GLfloat) * 6;
    
    int loc = lineShader->getAttribute("vs_position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, false, stride, 0);
    
    loc = lineShader->getAttribute("vs_color");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, false, stride, (GLvoid*)(3 * sizeof(GLfloat)));
}

void Renderer3D::resize(int width, int height)
{
    viewport.width = width;
    viewport.height = height;
    camera.setProjection(cgl::perspective(1.0471975512, 1, 0.1f, 100.0f));
}

void Renderer3D::draw()
{
    glBindVertexArray(vao);
    
    lineShader->enable();
    
    
    cgl::Mat4 mvp = camera.getProjection() * camera.getView() * model;
    glUniformMatrix4fv(lineShader->getUniform("modelViewProjection"), 1, false, mvp);
    
    
    glDrawArrays(GL_LINES, 0, numGridVerts);
}