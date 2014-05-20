#include "MenuItem.h"

MenuItem::MenuItem(std::string name, Action action) : name(name), action(action)
{
}

void MenuItem::trigger()
{
	if (action)
		action();
}

void MenuItem::setAction(Action action)
{
	this->action = action;
}

const std::string& MenuItem::getName() const
{
	return name;
}