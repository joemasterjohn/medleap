#ifndef __medleap__MenuController__
#define __medleap__MenuController__

#include "layers/Controller.h"
#include "MenuRenderer.h"
#include "gl/math/Math.h"
#include "leap/OneFingerTracker.h"
#include <memory>

class MenuController : public Controller
{
public:
	MenuController();
	~MenuController();

	MenuRenderer* getRenderer() override;
	bool keyboardInput(GLFWwindow* window, int key, int action, int mods) override;
	bool mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y) override;
	bool mouseMotion(GLFWwindow* window, double x, double y) override;
	bool scroll(GLFWwindow* window, double dx, double dy) override;
	bool leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame) override;
	void update(std::chrono::milliseconds elapsed) override;

	void hideMenu();

private:
	bool visible_;
    std::string workingDir;
	std::unique_ptr<Menu> menu_;
	MenuRenderer renderer;
	int selected_;
	OneFingerTracker finger_tracker_;

	int calcHighlightedMenu(double x, double y);
	int calcHighlightedMenu(double radians);    

	void showMainMenu();
	void showContextMenu();
};

#endif /* defined(__medleap__MenuController__) */
