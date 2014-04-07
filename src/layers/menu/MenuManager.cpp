#include "Menu.h"

Menu::Menu()
{
}

Menu::~Menu()
{
}

MenuScreen& Menu::createScreen(std::string name)
{
	screens.push_back(MenuScreen(name));
	return screens.back();
}