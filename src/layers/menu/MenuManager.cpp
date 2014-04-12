#include "MenuManager.h"
#include <algorithm>
#include <iostream>

using namespace std;

MenuManager::MenuManager() :
    transitionProgress(0),
    transitionDelta(0.008)
{
}

MenuManager::~MenuManager()
{
}

void MenuManager::reset()
{
	transitionProgress = 0;
	fadeIn();
}

Menu& MenuManager::top()
{
    return *(menuStack.top().get());
}

void MenuManager::fadeAway()
{
	tasks.push([this](std::chrono::milliseconds elapsed)->bool{
		transitionProgress -= transitionDelta * elapsed.count();
		if (transitionProgress <= 0.0) {
			transitionProgress = 0.0;
			return true;
		}
		return false;
	});
}

void MenuManager::fadeIn()
{
	tasks.push([this](std::chrono::milliseconds elapsed)->bool{
		transitionProgress += transitionDelta * elapsed.count();
		if (transitionProgress >= 1.0) {
			transitionProgress = 1.0;
			return true;
		}
		return false;
	});
}

void MenuManager::flash()
{
	fadeAway();
	fadeIn();
}

void MenuManager::push(std::shared_ptr<Menu> menu)
{
	if (menuStack.empty()) {
		menuStack.push(menu);
		fadeIn();
	} else {
		fadeAway();
		tasks.push([this, menu](std::chrono::milliseconds elapsed)->bool{
			menuStack.push(menu);
			return true;
		});
		fadeIn();
	}
}

void MenuManager::pop()
{
	if (menuStack.empty())
		return;

	fadeAway();
	tasks.push([this](std::chrono::milliseconds elapsed)->bool{
		menuStack.pop();
		return true;
	});
	fadeIn();
}

void MenuManager::update(std::chrono::milliseconds elapsed)
{
	// perform any tasks waiting to be done
	if (!tasks.empty()) {
		bool done = tasks.front()(elapsed);
		if (done)
			tasks.pop();
	}
}

bool MenuManager::isEmpty()
{
    return menuStack.empty();
}

double MenuManager::visibility()
{
	return transitionProgress;
}

void MenuManager::setLeapProgress(float p)
{
	this->leapProgress = p;
}

float MenuManager::getLeapProgress()
{
	return leapProgress;
}