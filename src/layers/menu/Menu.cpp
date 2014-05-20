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
	items_.push_back(MenuItem(name, action));
	return items_.back();
}

std::vector<MenuItem>& Menu::getItems()
{
	return items_;
}

const std::vector<MenuItem>& Menu::items() const
{
	return items_;
}

const std::string& Menu::getName() const
{
	return name;
}

MenuItem& Menu::operator[](int i)
{
	return items_[i];
}