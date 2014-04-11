#ifndef __medleap__MainMenu__
#define __medleap__MainMenu__

#include "Menu.h"
#include "MenuManager.h"

class MainMenu : public Menu
{
public:
    MainMenu(MenuManager* menus);
    ~MainMenu();
};

#endif /* defined(__medleap__MainMenu__) */
