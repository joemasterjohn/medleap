#ifndef medleap_Controller_h
#define medleap_Controller_h

#include "gl/glew.h"
#include <GLFW/glfw3.h>
#include "gl/Viewport.h"
#include "Leap.h"
#include <set>
#include <chrono>
#include <memory>
#include "layers/menu/Menu.h"

/** User interface controller. Controllers process input in the reverse order they were pushed onto the MainController stack. The controller may allow input to pass through to the next layer, or it may block/consume it such that no lower-level layers can take action. */
class Controller
{
public:
    /** Virtual destructor */
    virtual ~Controller() {}
    
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

	/** Draw layer */
	virtual void draw()
	{
	}

	/** If this layer has some context menu, it can be created here */
	virtual std::unique_ptr<Menu> contextMenu()
	{
		return nullptr;
	}

	/** If true, this controller will suppress any global Leap gestures like opening the menu */
	virtual bool modal() const {
		return false;
	}

	/** Called when this controller gains Leap focus */
	virtual void gainFocus()
	{
	}

	/** Called when this controller had Leap focus and is losing it */
	virtual void loseFocus()
	{
	}

	void setViewport(int x, int y, int width, int height)
	{
		bool changed = viewport_.width != width || viewport_.height != height;
		viewport_.x = x;
		viewport_.y = y;
		viewport_.width = width;
		viewport_.height = height;

		if (changed)
			resize();
	}

	const gl::Viewport& getViewport()
	{
		return viewport_;
	}

protected:
	gl::Viewport viewport_;

private:
	/** Rendering surface area is resized */
	virtual void resize()
	{
	}
};

#endif
