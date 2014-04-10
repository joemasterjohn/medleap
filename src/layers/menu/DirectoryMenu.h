#ifndef __medleap_DirectoryMenu__
#define __medleap_DirectoryMenu__

#include "Menu.h"
#include "MenuManager.h"

class DirectoryMenu : public Menu
{
public:
	DirectoryMenu(std::string* workingDir, MenuManager* menus);
	~DirectoryMenu();
    
private:
    std::string* workingDir;
    MenuManager* menus;
    
    void loadDirectory();
    void goUpDirectory();
    void goIntoDirectory(const std::string& subdir);
};

#endif // __medleap_DirectoryMenu__