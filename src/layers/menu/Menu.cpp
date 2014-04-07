#include "Menu.h"

Menu::Menu(std::string name) : name(name)
{
}

Menu::~Menu()
{
}

MenuItem& Menu::createItem(std::string name)
{
	items.push_back(MenuItem(name));
	return items.back();
}

std::vector<MenuItem>& Menu::getItems()
{
	return items;
}