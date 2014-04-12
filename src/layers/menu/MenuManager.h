#ifndef __medleap__MenuManager__
#define __medleap__MenuManager__

#include "Menu.h"
#include <stack>
#include <memory>
#include <chrono>
#include <queue>

/** Manages transitions between menus in a stack */
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
	void update(std::chrono::milliseconds elapsed);
    
    /** No active windows */
    bool isEmpty();

	/** 0 (invisible) to 1 (fully visible) */
	double visibility();

	void reset();

	void flash();

	void fadeAway();

	void fadeIn();

	float getLeapProgress();
	void setLeapProgress(float p);
    
private:
	/** A task that is updated over time. Returns TRUE if complete. */
	typedef std::function<bool(std::chrono::milliseconds elapsed)> Task;

	float leapProgress;
	std::queue<Task> tasks;
	std::stack<std::shared_ptr<Menu>> menuStack;
    double transitionProgress;
    double transitionDelta;
};

#endif /* defined(__medleap__MenuManager__) */
