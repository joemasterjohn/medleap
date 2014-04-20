#ifndef medleap_Controller_h
#define medleap_Controller_h

#include "gl/glew.h"
#include <GLFW/glfw3.h>
#include "Renderer.h"
#include "Leap.h"
#include <set>
#include <chrono>

/** User interface controller. Controllers process input in the reverse order they were pushed onto the MainController stack. The controller may allow input to pass through to the next layer, or it may block/consume it such that no lower-level layers can take action. */
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
    virtual bool mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y)
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

	/** Leap input handler. Returns TRUE if input should pass through to next controller; false if consumed in this controller. */
	virtual bool leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame)
	{
		return true;
	}

	/** Returns a set of all the Leap gestures this controller would like activated. The MainController will only track the required gestures from active controllers. */
	virtual std::set<Leap::Gesture::Type> requiredGestures()
	{
		// empty set unless overriden
		return std::set<Leap::Gesture::Type>();
	}

	/** Called before rendering with the elapsed milliseconds since previous update */
	virtual void update(std::chrono::milliseconds elapsed)
	{
	}
};

#endif
