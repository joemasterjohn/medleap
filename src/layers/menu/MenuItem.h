#ifndef __medleap__MenuItem__
#define __medleap__MenuItem__

#include <string>
#include <functional>

/** Effectively a button in a menu */
class MenuItem
{
public:
	/** A function to be called when the menu item is activated */
	typedef std::function<void(void)> Action;

	/** Creates a new menu item with an optional action in the constructor */
	MenuItem(std::string name, Action action = nullptr);

	/** Triggers the menu item, which will notify the listener */
	void trigger();

	/** Sets the action to be performed when the button is triggered */
	void setAction(Action action);

	/** Returns the menu item name */
	const std::string& getName() const;

private:
	std::string name;
	Action action;
};

#endif /* defined(__medleap__MenuItem__) */
