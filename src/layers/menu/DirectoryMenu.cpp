#include "DirectoryMenu.h"
#include "main/MainController.h"

#if defined(_WIN32)
#include "util/dirent.h"
#else
#include <dirent.h>
#endif
#include <errno.h>

DirectoryMenu::DirectoryMenu(std::string workingDir, MenuManager& menus) : 
		Menu(workingDir), 
		workingDir(workingDir),
		menus(menus)
{
	goIntoDir(workingDir, false);
}

DirectoryMenu::~DirectoryMenu()
{
}

void DirectoryMenu::goIntoDir(const std::string& directory, bool flash)
{
	workingDir = directory;
	name = workingDir;
	items.clear();

	if (flash)
		menus.flash();

	MenuItem& mi = createItem("<CANCEL>");
	mi.setAction([this]{ menus.pop(); });

	DIR* dir = opendir(workingDir.c_str());
	struct dirent* entry = readdir(dir);
	
	while (entry != NULL)
	{
		if (entry->d_type == DT_DIR) {
			std::string subdir(entry->d_name);
			if (subdir == ".") {
				MenuItem& mi = this->createItem("<LOAD>");
				mi.setAction([this]{loadDir();});
			} else if (subdir == "..") {
				MenuItem& mi = this->createItem("<UP>");
				mi.setAction([this]{goUpDir();});
			} else {
				MenuItem& mi = createItem(subdir);
				mi.setAction([this,subdir]{goIntoDir(workingDir + "/" + subdir, true);});
			}
		}
		entry = readdir(dir);
	}
	closedir(dir);
}

void DirectoryMenu::goUpDir()
{
	int i = workingDir.rfind("/");
	std::string upDir = (i == 0) ? "/" : workingDir.substr(0, i);
	goIntoDir(upDir, true);
}

void DirectoryMenu::loadDir()
{
	MainController::getInstance().setVolumeToLoad(workingDir);
	menus.pop();
}