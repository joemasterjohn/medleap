#ifndef __medleap__Menu__
#define __medleap__Menu__

#include <vector>
#include "MenuItem.h"

class Menu
{
public:
	/** Creates a named menu */
	Menu(std::string name);

	/** Destructor for menu */
	virtual ~Menu();
	
	/** Creates a new menu item and returns a reference to it. */
	MenuItem& createItem(std::string name);

	/** Creates a new menu item with an action and returns a reference to it. */
	MenuItem& createItem(std::string name, MenuItem::Action action);

	/** Returns the menu items stored in the menu */
	std::vector<MenuItem>& getItems();

	/** Returns the menu's name */
	const std::string& getName() const;
    
    /** Returns the menu at index i */
    MenuItem& operator[](int i);

protected:
	std::string name;
	std::vector<MenuItem> items;
};

#endif /* defined(__medleap__Menu__) */
