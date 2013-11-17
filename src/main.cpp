#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "gl/shader.h"
#include "gl/program.h"
#include "volume/DCMImageSeries.h"
#include "gl/TextRenderer.h"

GLFWwindow* window;
GLuint vao;
GLuint vbo;
Program* program;
GLuint texture = 0;
int iWindow = 0;
VolumeData* myVolume;
TextRenderer tr;

void init()
{
    program = Program::create("shaders/ct_mip.vert",
                              "shaders/ct_mip.frag");
    
    tr.load("fonts/menlo18.bmp", "fonts/menlo18.dat");
    
    program->enable();
    
    glGenTextures(1, &texture);
    myVolume->loadTexture2D(texture, 0);


    const GLubyte* str = glGetString(GL_VERSION);
    std::cout << "Version = " << str << std::endl;
    
    // create a VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // create a VBO
    GLfloat data[] = { -1, -1, 0, 0,
                        1, -1, 1, 0,
                        1,  1, 1, 1,
                       -1, -1, 0, 0,
                        1,  1, 1, 1,
                        -1,  1, 0, 1};
    
    GLsizei stride = 4 * sizeof(GLfloat);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    
    int loc = program->getAttribute("vPosition");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, 0);
    
    loc = program->getAttribute("vTexCoord");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, (GLvoid*)(2 * sizeof(GLfloat)));
}

void reshape(int width, int height)
{
    
}

void display()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    program->enable();

    int ww, wh;
    glfwGetWindowSize(window, &ww, &wh);
    float windowAspect = (float)ww/wh;
    float imgAspect = (float)myVolume->getWidth() / myVolume->getHeight();
    if (imgAspect <= 1.0f) {
        float sx = imgAspect / windowAspect;
        float sy = 1.0f;
        float model[] = {
            sx, 0.0f, 0.0f, 0.0f,
            0.0f, sy, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f };
        glUniformMatrix4fv(program->getUniform("model"), 1, false, model);
    } else {
        float sx = 1.0f;
        float sy = windowAspect / imgAspect;
        float model[] = {
            sx, 0.0f, 0.0f, 0.0f,
            0.0f, sy, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f };
        glUniformMatrix4fv(program->getUniform("model"), 1, false, model);
    }
    
    // set the window
    float w = myVolume->getWindows()[iWindow].getWidthNormalized(myVolume->getType());
    float c = myVolume->getWindows()[iWindow].getCenterNormalized(myVolume->getType());
    glUniform1i(program->getUniform("signed_normalized"), myVolume->isSigned());
    glUniform1f(program->getUniform("window_min"), c - w/2.0f);
    glUniform1f(program->getUniform("window_multiplier"), 1.0f / w);
    
    // set state and shader for drawing medical stuff
    GLsizei stride = 4 * sizeof(GLfloat);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    int loc = program->getAttribute("vPosition");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, 0);
    loc = program->getAttribute("vTexCoord");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, (GLvoid*)(2 * sizeof(GLfloat)));
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        tr.setColor(1, 1, 0.5);
        tr.begin(width, height);
        tr.draw("this is a message", 100, 100);
        tr.draw("another text", 100, 200);
        tr.draw("abc", 600, 500);
        tr.end();
    }
}

void keyboardCB(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    static int curImage = 0;
    if (key == GLFW_KEY_RIGHT/* && action == GLFW_PRESS */) {
        curImage = (curImage + 1) % myVolume->getDepth();
        std::cout << curImage << std::endl;
        myVolume->loadTexture2D(texture, curImage);
      
    }
    
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        iWindow = (iWindow + 1) % myVolume->getWindows().size();
    }
    
}

bool down = false;

void mouseCB(GLFWwindow* window, int button, int action, int mods)
{
    int x, y;
    glfwGetWindowPos(window, &x, &y);
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        down = action == GLFW_PRESS;
    }
}

void cursorCB(GLFWwindow* window, double x, double y)
{
}

bool initWindow(int width, int height, const char* title)
{
    if (!glfwInit())
        return false;
    
    // Use 32-bit color (in sRGB) and 24-bit for depth buffer
    glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    
    // Use OpenGL 3.2 core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create a window
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    
	// Set window callback functions for events
    //glfwSetFramebufferSizeCallback(window, resizeCB);
	glfwSetKeyCallback(window, keyboardCB);
	glfwSetMouseButtonCallback(window, mouseCB);
    glfwSetCursorPosCallback(window, cursorCB);
    
    // Set vertical retrace rate (0 == run as fast as possible)
    //glfwSwapInterval(vsync);
    
    // Initialize GLEW, which provides access to OpenGL functions / extensions
	glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        glfwTerminate();
        return false;
    }
    
    // Let program initialize OpenGL resources
    init();
    reshape(width, height);
    
    // Start rendering loop
    while (!glfwWindowShouldClose(window)) {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return true;
}

int main(int argc, char** argv)
{    
    if (argc != 2) {
        std::cout << "provide dicom directory as argument" << std::endl;
        return 0;
    }
    
    
    myVolume = DCMImageSeries::load(argv[1]);
    initWindow(800, 600, "hello world");
    
    
    
    return 0;
}
