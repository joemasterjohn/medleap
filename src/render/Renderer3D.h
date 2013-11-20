#ifndef __medleap__Renderer3D__
#define __medleap__Renderer3D__

#include <GL/glew.h>
#include "math/Matrix4.h"
#include "gl/Viewport.h"
#include "gl/Program.h"
#include "util/Camera.h"

class Renderer3D
{
public:
    Renderer3D();
    ~Renderer3D();
    void init();
    void resize(int width, int height);
    void draw();
    
    cgl::Camera& getCamera();

private:
    cgl::Camera camera;
    cgl::Viewport viewport;
    cgl::Mat4 model;
    
    // grid
    Program* lineShader;
    GLuint vao;
    GLuint vbo;
    int numGridVerts;
};

#endif /* defined(__medleap__Renderer3D__) */