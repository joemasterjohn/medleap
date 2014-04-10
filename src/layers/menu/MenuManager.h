#ifndef __medleap__MenuManager__
#define __medleap__MenuManager__

#include "Menu.h"
#include <stack>
#include <memory>

class MenuManager
{
public:
    
	MenuManager();
    
	~MenuManager();
    
    /** Returns a reference to the menu on top of the stack */
	Menu& top();
    
    /** Pushes a new menu on top */
	void push(std::shared_ptr<Menu> menu);
    
    /** Pops the top menu from the stack */
	void pop();

    /** Updates menu transitions */
	void update();
    
    /** No active windows */
    bool isEmpty();
    
private:
	std::stack<std::shared_ptr<Menu> > menuStack;
    std::shared_ptr<Menu> newTop;
    double transitionProgress;
    double transitionDelta;
};

#endif /* defined(__medleap__MenuManager__) */
