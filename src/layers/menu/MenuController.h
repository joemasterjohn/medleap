#ifndef __medleap__MenuController__
#define __medleap__MenuController__

#include "layers/Controller.h"
#include "MenuRenderer.h"
#include "MenuManager.h"
#include "gl/math/Vector2.h"

class MenuController : public Controller
{
public:
	MenuController();
	~MenuController();
	MenuRenderer* getRenderer();
	MenuManager& getMenuManager();
	bool keyboardInput(GLFWwindow* window, int key, int action, int mods);
	bool mouseButton(GLFWwindow* window, int button, int action, int mods);
	bool mouseMotion(GLFWwindow* window, double x, double y);
	bool scroll(GLFWwindow* window, double dx, double dy);
	bool leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame);
	void update(std::chrono::milliseconds elapsed);
	void setLeapCenter(const Vec2& center);

private:
	Vec2 leapCenter;
    std::string workingDir;
	MenuManager menus;
	MenuRenderer* renderer;

	int calcHighlightedMenu(double x, double y);
	int calcHighlightedMenu(double radians);    
};

#endif /* defined(__medleap__MenuController__) */
