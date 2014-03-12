#include "MainRenderer.h"

MainRenderer::MainRenderer()
{
}

MainRenderer::~MainRenderer()
{
}

GLFWwindow* MainRenderer::getWindow()
{
    return window;
}

bool MainRenderer::init(int width, int height, const char* title)
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
    
    // Set vertical retrace rate (0 == run as fast as possible)
    //glfwSwapInterval(vsync);
    
    // Initialize GLEW, which provides access to OpenGL functions / extensions
	glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        glfwTerminate();
        return false;
    }
    
    this->width = width;
    this->height = height;
    
    return true;
}

void MainRenderer::draw()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // draw from stack
    for (RenderLayer* r : activeLayers) {
        r->draw();
    }
    
    glfwSwapBuffers(window);
}

void MainRenderer::pushLayer(RenderLayer* layer)
{
    activeLayers.push_back(layer);
}

RenderLayer* MainRenderer::popLayer()
{
    if (activeLayers.empty())
        return NULL;
    RenderLayer* popped = activeLayers.back();
    activeLayers.pop_back();
    
    return popped;
}

void MainRenderer::clearLayers()
{
    while (!activeLayers.empty())
        activeLayers.pop_back();
}

int MainRenderer::getWidth()
{
    return width;
}

int MainRenderer::getHeight()
{
    return height;
}

void MainRenderer::resize(int width, int height)
{
    this->width = width;
    this->height = height;
}
