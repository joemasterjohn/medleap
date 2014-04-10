#include "MenuManager.h"

MenuManager::MenuManager() :
    newTop(nullptr),
    transitionProgress(0),
    transitionDelta(0.01)
{
}

MenuManager::~MenuManager()
{
}

Menu& MenuManager::top()
{
    
    return *(menuStack.top().get());
}

void MenuManager::push(std::shared_ptr<Menu> menu)
{
//    newTop = menu;
    menuStack.push(menu);
}

void MenuManager::pop()
{
	menuStack.pop();
//    newTop = menuStack.top();
}

void MenuManager::update()
{
//    if (newTop) {
//        transitionProgress -= transitionDelta;
//        if (transitionProgress <= 0) {
//            transitionProgress = 0;
//            menuStack.push(newTop);
//            newTop = nullptr;
//        }
//    } else if (transitionProgress < 1.0) {
//        transitionProgress += transitionDelta;
//        if (transitionProgress > 1.0)
//            transitionProgress = 1.0;
//    }
}

bool MenuManager::isEmpty()
{
    return menuStack.empty();
}