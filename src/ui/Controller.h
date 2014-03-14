#ifndef medleap_Controller_h
#define medleap_Controller_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "render/Renderer.h"

/** User interface controller. Controllers operate on a single render layer. Controllers process user input top-to-bottom starting with the highest-level render layer. The user input may pass through this controller to the next layer only if the respective functions allow it (by returning TRUE); otherwise, the input is "consumed" and will not reach lower-level layers. A controller may have "children" controllers. */
class Controller
{
public:
    /** Virtual destructor */
    virtual ~Controller() {}
    
    /** Returns a pointer to the controller's renderer, or NULL if this controller doesn't have one */
    virtual Renderer* getRenderer() = 0;
    
    /** Keyboard input handler. Returns TRUE if input should pass through to next controller; false if consumed in this controller. */
    virtual bool keyboardInput(GLFWwindow* window, int key, int action, int mods)
    {
        return true;
    }
    
    /** Mouse button input handler. Returns TRUE if input should pass through to next controller; false if consumed in this controller. */
    virtual bool mouseButton(GLFWwindow* window, int button, int action, int mods)
    {
        return true;
    }
    
    /** Mouse motion input handler. Returns TRUE if input should pass through to next controller; false if consumed in this controller. */
    virtual bool mouseMotion(GLFWwindow* window, double x, double y)
    {
        return true;
    }
    
    /** Mouse scroll input handler. Returns TRUE if input should pass through to next controller; false if consumed in this controller. */
    virtual bool scroll(GLFWwindow* window, double dx, double dy)
    {
        return true;
    }
};

#endif
