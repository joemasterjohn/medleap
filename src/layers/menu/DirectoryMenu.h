#ifndef __medleap_DirectoryMenu__
#define __medleap_DirectoryMenu__

#include "Menu.h"
#include "MenuManager.h"
#include "data/VolumeLoader.h"

class DirectoryMenu : public Menu
{
public:
	DirectoryMenu(std::string workingDir, MenuManager& menus);
	~DirectoryMenu();
        
private:
    std::string workingDir;
	MenuManager& menus;

	void goIntoDir(const std::string& subDir, bool flash);
	void goUpDir();
	void load(const VolumeLoader::Source& source);
};

#endif // __medleap_DirectoryMenu__