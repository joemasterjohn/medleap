#ifndef __medleap__MenuController__
#define __medleap__MenuController__

#include "layers/Controller.h"
#include "MenuRenderer.h"
#include "MenuManager.h"

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

private:
	MenuManager menuManager;
	MenuRenderer* renderer;

	int calcHighlightedMenu(double x, double y);
};

#endif /* defined(__medleap__MenuController__) */
