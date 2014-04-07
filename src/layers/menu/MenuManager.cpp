#include "MenuManager.h"

MenuManager::MenuManager()
{
}

MenuManager::~MenuManager()
{
	for (Menu* menu : menus)
		delete menu;
}

Menu& MenuManager::createMenu(std::string name)
{
	// not sure if I like this... maybe get rid of it and have new menus pushed/popped only
	Menu* menu = new Menu(name);
	menus.push_back(menu);
	menuMap[name] = menu;
	return *menu;
}

Menu& MenuManager::topMenu()
{
	return *(menuStack.top());
}

const std::vector<Menu*>& MenuManager::getMenus() const
{
	return menus;
}

void MenuManager::pushMenu(const std::string& name)
{
	// not checking for errors bad
	Menu* menu = menuMap[name];
	menuStack.push(menu);
}

void MenuManager::popMenu()
{
	menuStack.pop();
}