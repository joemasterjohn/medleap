#ifndef __medleap__MainRenderer__
#define __medleap__MainRenderer__

#include "gl/glew.h"
#include <GLFW/glfw3.h>
#include "layers/Controller.h"
#include <list>
#include "gl/math/Math.h"

/** Controls all rendering. Should be created once upon application start. */
class MainRenderer
{
public:
    MainRenderer();
    
    ~MainRenderer();
    
    /** Draws all layers in order */
    void draw(int width, int height);
    
    /** Pushes a render layer to the drawing stack. */
    void pushLayer(Controller* layer);
    
    /** Pushes a render layer and docks it to the left. All non-docked,lower-level layers will be pushed right. */
	void dockLeft(Controller* layer, double percentWidth);
    
    /** Pushes a render layer and docks it to the right. All non-docked, lower-level layers will be pushed left. */
	void dockRight(Controller* layer, double percentWidth);
    
    /** Pushes a render layer and docks it to the bottom. All non-docked, lower-level layers will be pushed up. */
	void dockBottom(Controller* layer, double percentHeight);
    
    /** Pushes a render layer and docks it to the top. All non-docked, lower-level layers will be pushed down. */
	void dockTop(Controller* layer, double percentHeight);
    
    /** Pops the top layer from the drawing stack. Returns the popped layer or NULL if empty. */
	Controller* popLayer();
    
    /** Pops all layers off the drawing stack. */
    void clearLayers();

	gl::Vec3 getInverseBGColor() const;
	gl::Vec3 getBackgroundColor() const;
	void setBackgroundColor(const gl::Vec3& c);
	void setLightBG();
	void setDarkBG();
    
private:
	std::list<Controller*> activeLayers;
	GLuint vao;
	gl::Vec3 bgColor;
    
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
    
	void updateViewport(Controller* renderer, int layer, int width, int height);
};

#endif /* defined(__medleap__MainRenderer__) */
