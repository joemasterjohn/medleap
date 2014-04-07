#ifndef __medleap__MenuManager__
#define __medleap__MenuManager__

#include "Menu.h"

class MenuManager
{
public:
	MenuManager();
	~MenuManager();

	MenuScreen& createScreen(std::string name);

	//void push();
	//void pop();
	//void update();

private:
	std::vector<MenuManagerScreen> screens;
};

#endif /* defined(__medleap__MenuManager__) */
