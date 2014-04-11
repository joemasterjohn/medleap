#include "DirectoryMenu.h"
#include "main/MainController.h"

#if defined(_WIN32)
#include "util/dirent.h"
#else
#include <dirent.h>
#endif


DirectoryMenu::DirectoryMenu(std::string workingDir, MenuManager& menus) : 
		Menu(workingDir), 
		workingDir(workingDir),
		menus(menus)
{
	goIntoDir(workingDir);
}

DirectoryMenu::~DirectoryMenu()
{
}

void DirectoryMenu::goIntoDir(const std::string& directory)
{
	workingDir = directory;
	name = workingDir;

	// modify working dir, set new items
	items.clear();

	// add all the subdirectories as items in this menu
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
				mi.setAction([this,subdir]{goIntoDir(workingDir + "/" + subdir);});
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
	goIntoDir(upDir);
}

void DirectoryMenu::loadDir()
{
	MainController::getInstance().setVolumeToLoad(workingDir);
	menus.pop();
}