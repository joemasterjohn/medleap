#include "DirectoryMenu.h"
#include "main/MainController.h"

#if defined(_WIN32)
#include "util/dirent.h"
#else
#include <dirent.h>
#endif


DirectoryMenu::DirectoryMenu(std::string* workingDir, MenuManager* menus) : Menu(*workingDir), workingDir(workingDir), menus(menus)
{
	// add all the subdirectories as items in this menu
	DIR* dir = opendir(workingDir->c_str());
	struct dirent* entry = readdir(dir);
	while (entry != NULL)
	{
		if (entry->d_type == DT_DIR) {
            std::string subdir(entry->d_name);
			MenuItem& mi = this->createItem(subdir);
			if (subdir == ".") {
                mi.setAction([this]{this->loadDirectory();});
			} else if (subdir == "..") {
                mi.setAction([this]{this->goUpDirectory();});
			} else {
                mi.setAction([this, subdir]{this->goIntoDirectory(subdir);});
			}
		}
		entry = readdir(dir);
	}
	closedir(dir);

	//createItem("<CANCEL>", [&menus]{menus.pop(); });
}

void DirectoryMenu::loadDirectory()
{
    MainController::getInstance().setVolumeToLoad(*workingDir);
    menus->pop();
}

void DirectoryMenu::goUpDirectory()
{
    int i = workingDir->rfind("/");
    *workingDir = (i == 0) ? "/" : workingDir->substr(0, i);
    menus->pop();
    menus->push(std::shared_ptr<DirectoryMenu>(new DirectoryMenu(workingDir, menus)));
}

void DirectoryMenu::goIntoDirectory(const std::string& subdir)
{
    *workingDir = *workingDir + "/" + subdir;
    menus->pop();
    menus->push(std::shared_ptr<DirectoryMenu>(new DirectoryMenu(workingDir, menus)));
}

DirectoryMenu::~DirectoryMenu()
{
}