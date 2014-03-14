#ifndef __medleap__MainRenderer__
#define __medleap__MainRenderer__

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "render/Renderer.h"
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
    
    /** Pushes a render layer to the drawing stack. */
    void pushLayer(Renderer* layer);
    
    /** Pushes a render layer and docks it to the left. All non-docked,lower-level layers will be pushed right. */
    void dockLeft(Renderer* layer, double percentWidth);
    
    /** Pushes a render layer and docks it to the right. All non-docked, lower-level layers will be pushed left. */
    void dockRight(Renderer* layer, double percentWidth);
    
    /** Pushes a render layer and docks it to the bottom. All non-docked, lower-level layers will be pushed up. */
    void dockBottom(Renderer* layer, double percentHeight);
    
    /** Pushes a render layer and docks it to the top. All non-docked, lower-level layers will be pushed down. */
    void dockTop(Renderer* layer, double percentHeight);
    
    /** Pops the top layer from the drawing stack. Returns the popped layer or NULL if empty. */
    Renderer* popLayer();
    
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
    std::list<Renderer*> activeLayers;
    GLFWwindow* window;
    int width;
    int height;
    
    /** A docked layer */
    struct Docking
    {
        /** level of the stack the dock was pushed to (-1 if nothing docked) */
        int layerIndex;
        
        /** how much of the width (docked left/right) or height (docked bottom/top) to occupy */
        double percent;
    };
    
    Docking leftDocking;
    Docking rightDocking;
    Docking bottomDocking;
    Docking topDocking;
    
    void applyDocking(int layer);
};

#endif /* defined(__medleap__MainRenderer__) */
