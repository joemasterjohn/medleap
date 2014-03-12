#ifndef __medleap__MainRenderer__
#define __medleap__MainRenderer__

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "render/RenderLayer.h"
#include <list>

/** Controls all rendering. Should be created once upon application start. */
class MainRenderer
{
public:
    MainRenderer();
    
    ~MainRenderer();
    
    /** Initializes OpenGL context and window. Returns FALSE on failure. */
    bool init(int width, int height, const char* title);
    
    /** Draws all layers in order */
    void draw();
    
    /** Pushes a render layer to the drawing stack. This will be the last layer drawn. */
    void pushLayer(RenderLayer* layer);
    
    /** Pops the top layer from the drawing stack. Returns the popped layer or NULL if empty. */
    RenderLayer* popLayer();
    
    /** Pops all layers off the drawing stack. */
    void clearLayers();
    
    /** Returns a pointer to the main window */
    GLFWwindow* getWindow();
    
    /** Width of rendering surface. */
    int getWidth();
    
    /** Height of rendering surface. */
    int getHeight();
    
    /** Window resized event */
    void resize(int width, int height);
    
private:
    std::list<RenderLayer*> activeLayers;
    GLFWwindow* window;
    int width;
    int height;
};

#endif /* defined(__medleap__MainRenderer__) */
