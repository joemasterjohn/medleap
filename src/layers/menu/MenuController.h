#ifndef __medleap__MenuController__
#define __medleap__MenuController__

#include "layers/Controller.h"
#include "MenuRenderer.h"
#include "MenuManager.h"
#include "gl/math/Math.h"

class MenuController : public Controller
{
public:
	MenuController();
	~MenuController();
	MenuRenderer* getRenderer();
	MenuManager& getMenuManager();
	bool keyboardInput(GLFWwindow* window, int key, int action, int mods) override;
	bool mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y) override;
	bool mouseMotion(GLFWwindow* window, double x, double y) override;
	bool scroll(GLFWwindow* window, double dx, double dy) override;
	bool leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame) override;
	void update(std::chrono::milliseconds elapsed);
	void setLeapCenter(const gl::Vec2& center);

private:
	gl::Vec2 leapCenter;
    std::string workingDir;
	MenuManager menus;
	MenuRenderer* renderer;

	int calcHighlightedMenu(double x, double y);
	int calcHighlightedMenu(double radians);    
};

#endif /* defined(__medleap__MenuController__) */
