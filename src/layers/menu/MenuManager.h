#ifndef __medleap__MenuManager__
#define __medleap__MenuManager__

#include "Menu.h"
#include <stack>
#include <unordered_map>

class MenuManager
{
public:
	MenuManager();
	~MenuManager();

	Menu& createMenu(std::string name);

	const std::vector<Menu*>& getMenus() const;

	Menu& topMenu();
	void pushMenu(const std::string& name);
	void popMenu();

	//void update();

private:
	std::vector<Menu*> menus;
	std::stack<Menu*> menuStack;
	std::unordered_map<std::string, Menu*> menuMap;
};

#endif /* defined(__medleap__MenuManager__) */
