#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "gl/shader.h"
#include "gl/program.h"
#include "volume/DCMImageSeries.h"

GLFWwindow* window;
GLuint vao;
GLuint vbo;
Program* program;
GLuint texture = 0;
VolumeData* myVolume;

void init()
{
    program = Program::create("shaders/ct_mip.vert",
                              "shaders/ct_mip.frag");
    
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
    glClear(GL_COLOR_BUFFER_BIT);
    
    // set the window
    float w = myVolume->windows[0].getWidthNormalized(myVolume->type);
    float c = myVolume->windows[0].getCenterNormalized(myVolume->type);
    glUniform1i(program->getUniform("signed_normalized"), myVolume->isSigned());
    glUniform1f(program->getUniform("window_min"), c - w/2.0f);
    glUniform1f(program->getUniform("window_multiplier"), 1.0f / w);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void keyboardCB(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    static int curImage = 0;
    if (key == GLFW_KEY_RIGHT/* && action == GLFW_PRESS */) {
        curImage = (curImage + 1) % myVolume->depth;
        std::cout << curImage << std::endl;
        myVolume->loadTexture2D(texture, curImage);
      
    }
    
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        myVolume->windows[0].setCenter(myVolume->windows[0].getCenter() + .0002);
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
    if (down) {
        
        // maybe show histogram while dragging?
        if (x < 0)
            myVolume->windows[0].setCenter(myVolume->windows[0].getCenter() - .0002);
        else if (x > 0)
            myVolume->windows[0].setCenter(myVolume->windows[0].getCenter() + .0002);
        
        if (y < 0)
            myVolume->windows[0].setWidth(myVolume->windows[0].getWidth() - .0002);
        else if (y > 0)
            myVolume->windows[0].setWidth(myVolume->windows[0].getWidth() + .0002);
    }
    
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
    initWindow(600, 600, "hello world");
    
    return 0;
}
