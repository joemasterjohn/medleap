#ifndef __medleap__Menu__
#define __medleap__Menu__

#include <vector>
#include "MenuItem.h"

class Menu
{
public:
	/** Creates a named menu screen */
	Menu(std::string name);

	/** Destructor for menu screen */
	~Menu();
	
	/** Creates a new menu item and returns a reference to it. */
	MenuItem& createItem(std::string name);

	/** Returns the menu items stored in the menu */
	std::vector<MenuItem>& getItems();

private:
	std::string name;
	std::vector<MenuItem> items;
};

#endif /* defined(__medleap__Menu__) */
