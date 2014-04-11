#ifndef __medleap_DirectoryMenu__
#define __medleap_DirectoryMenu__

#include "Menu.h"
#include "MenuManager.h"

class DirectoryMenu : public Menu
{
public:
	DirectoryMenu(std::string workingDir, MenuManager& menus);
	~DirectoryMenu();
    
//    const std::string& getFullPath();
    
private:
    std::string workingDir;
	MenuManager& menus;

	void goIntoDir(const std::string& subDir);
	void goUpDir();
	void loadDir();
};

#endif // __medleap_DirectoryMenu__