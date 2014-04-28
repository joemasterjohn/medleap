#include "DirectoryMenu.h"
#include "main/MainController.h"

#if defined(_WIN32)
#include "util/dirent.h"
#else
#include <dirent.h>
#endif
#include <errno.h>

DirectoryMenu::DirectoryMenu(std::string workingDir) : 
		Menu(workingDir), 
		workingDir(workingDir)
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

	//if (flash)
	//	menus.flash();

	MenuItem& mi = createItem("<CANCEL>");
	//mi.setAction([this]{ menus.pop(); });

	DIR* dir = opendir(workingDir.c_str());
	struct dirent* entry = readdir(dir);
	
	while (entry != NULL)
	{
		if (entry->d_type == DT_DIR) {
			std::string subdir(entry->d_name);
			if (subdir == ".") {
				MenuItem& mi = this->createItem("<LOAD>");
				VolumeLoader::Source src = { workingDir, VolumeLoader::Source::DICOM_DIR };
				mi.setAction([this, src]{load(src);});
			} else if (subdir == "..") {
				MenuItem& mi = this->createItem("<UP>");
				mi.setAction([this]{goUpDir();});
			} else {
				MenuItem& mi = createItem(subdir);
				mi.setAction([this,subdir]{goIntoDir(workingDir + "/" + subdir, true);});
			}
		} else {
			std::string fileName(entry->d_name);
			if (fileName.size() > 3 && fileName.substr(fileName.size() - 4, 4) == ".raw") {
				MenuItem& mi = this->createItem(fileName);
				VolumeLoader::Source src = { workingDir + "/" + fileName, VolumeLoader::Source::RAW };
				mi.setAction([this, src]{load(src); });
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

void DirectoryMenu::load(const VolumeLoader::Source& source)
{
	MainController::getInstance().setVolumeToLoad(source);
	//menus.pop();
	//MainController::getInstance().menuController().menu();
	//MainController::getInstance().showMenu(false);
}