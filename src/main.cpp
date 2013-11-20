#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "volume/DCMImageSeries.h"
#include "render/SliceRenderer2D.h"
#include "render/Renderer3D.h"
#include "ui/UIController.h"

SliceRenderer2D renderer2D;
GLFWwindow* window;
DCMImageSeries* myVolume;
UIController* controller;

Renderer3D renderer3D;

void init()
{
    renderer3D.init();
    
//    renderer2D.init();
//    renderer2D.setVolume(myVolume);
}

void reshape(int width, int height)
{
//    renderer2D.resize(width, height);
    renderer3D.resize(width, height);
}

void display()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
//    renderer2D.draw();
    
    renderer3D.draw();
}

void keyboardCB(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    controller->keyboardInput(window, key, action, mods);
}

void resizeCB(GLFWwindow* window, int width, int height)
{
    renderer2D.resize(width, height);
}

void mouseCB(GLFWwindow* window, int button, int action, int mods)
{
    controller->mouseButton(window, button, action, mods);
}

void cursorCB(GLFWwindow* window, double x, double y)
{
    controller->mouseMotion(window, x, y);
}

void scrollCB(GLFWwindow* window, double dx, double dy)
{
    controller->scroll(window, dx, dy);
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
    glfwWindowHint(GLFW_SAMPLES, 8);
    
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
    glfwSetFramebufferSizeCallback(window, resizeCB);
	glfwSetKeyCallback(window, keyboardCB);
	glfwSetMouseButtonCallback(window, mouseCB);
    glfwSetCursorPosCallback(window, cursorCB);
    glfwSetScrollCallback(window, scrollCB);
    
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
    if (!myVolume) {
        std::cout << "could not find volume " << argv[1] << std::endl;
        return 0;
    }
    
    controller = new UIController(&renderer3D);
    
    initWindow(800, 600, "hello world");
    
    return 0;
}
