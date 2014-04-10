#include "DirectoryMenu.h"
#include "main/MainController.h"

#if defined(_WIN32)
#include "util/dirent.h"
#else
#include <dirent.h>
#endif


DirectoryMenu::DirectoryMenu(std::string& workingDir, MenuManager& menus) : Menu(workingDir)
{
	// add all the subdirectories as items in this menu
	DIR* dir = opendir(workingDir.c_str());
	struct dirent* entry = readdir(dir);
	while (entry != NULL)
	{
		if (entry->d_type == DT_DIR) {
			std::string subdir(entry->d_name);
			MenuItem& mi = this->createItem(subdir);

			if (subdir == ".") {
				// if user presses on . menu item, use this directory to load a volume
				std::string toLoad = workingDir;
				mi.setAction([&menus, toLoad] {
					MainController::getInstance().setVolumeToLoad(toLoad);
					menus.pop();
				});
			}
			else if (subdir == "..") {
				// if user presses on .. menu item, go up one directory
				mi.setAction([&workingDir, &menus] {
					int i = workingDir.rfind("/");
					workingDir = (i == 0) ? "/" : workingDir.substr(0, i);
					menus.pop();
					menus.push(std::shared_ptr<DirectoryMenu>(new DirectoryMenu(workingDir, menus)));
				});
			}
			else {
				// otherwise, go deeper into the file system
				mi.setAction([&menus, subdir, &workingDir] {
					workingDir += "/" + subdir;
					menus.pop();
					menus.push(std::shared_ptr<DirectoryMenu>(new DirectoryMenu(workingDir, menus)));
				});
			}


		}
		entry = readdir(dir);
	}
	closedir(dir);

	createItem("<CANCEL>", [&menus]{menus.pop(); });
}

DirectoryMenu::~DirectoryMenu()
{
}