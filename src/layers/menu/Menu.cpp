#include "Menu.h"

Menu::Menu(std::string name) : name(name)
{
}

Menu::~Menu()
{
}

MenuItem& Menu::createItem(std::string name)
{
	return createItem(name, nullptr);
}

MenuItem& Menu::createItem(std::string name, MenuItem::Action action)
{
	items.push_back(MenuItem(name, action));
	return items.back();
}

std::vector<MenuItem>& Menu::getItems()
{
	return items;
}

const std::string& Menu::getName() const
{
	return name;
}