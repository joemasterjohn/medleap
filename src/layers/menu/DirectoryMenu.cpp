#include "DirectoryMenu.h"
#include "main/MainController.h"

#if defined(_WIN32)
#include "util/dirent.h"
#else
#include <dirent.h>
#endif


DirectoryMenu::DirectoryMenu(std::string workingDir, MenuManager* menus) : Menu(workingDir)
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
                // use working directory as a volume data source
                mi.setAction([=]{
                    MainController::getInstance().setVolumeToLoad(workingDir);
                    menus->pop();
                });
			} else if (subdir == "..") {
                // go up one directory
                mi.setAction([=]{
                    int i = workingDir.rfind("/");
                    std::string upDir = (i == 0) ? "/" : workingDir.substr(0, i);
                    menus->pop();
                    menus->push(std::shared_ptr<DirectoryMenu>(new DirectoryMenu(upDir, menus)));
                });
			} else {
                // go into subdir
                mi.setAction([=]{
                    std::string fullSubDir = workingDir + "/" + subdir;
                    menus->pop();
                    menus->push(std::shared_ptr<DirectoryMenu>(new DirectoryMenu(fullSubDir, menus)));
                });
			}
		}
		entry = readdir(dir);
	}
	closedir(dir);

	//createItem("<CANCEL>", [&menus]{menus.pop(); });
}

DirectoryMenu::~DirectoryMenu()
{
}