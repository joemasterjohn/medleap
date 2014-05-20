#include "DirectoryMenu.h"
#include "main/MainController.h"

#if defined(_WIN32)
#include "util/dirent.h"
#else
#include <dirent.h>
#endif
#include <errno.h>

DirectoryMenu::DirectoryMenu() : 
	Menu("Directory Menu"), 
	on_load_(nullptr)
{
}

void DirectoryMenu::directory(const std::string& directory_name)
{
	working_dir_ = directory_name;
	name = directory_name;

	items_.clear();

	DIR* dir = opendir(working_dir_.c_str());
	struct dirent* entry = readdir(dir);

	while (entry != NULL)
	{
		if (entry->d_type == DT_DIR) {
			std::string subdir(entry->d_name);
			if (subdir != "." && subdir != "..") {
				MenuItem& mi = createItem(subdir);
				mi.setAction([this, subdir]{directory(working_dir_ + "/" + subdir); });
			}
		} else {
			std::string fileName(entry->d_name);
			if (fileName.size() > 3 && fileName.substr(fileName.size() - 4, 4) == ".raw") {
				MenuItem& mi = this->createItem(fileName);
				VolumeLoader::Source src = { working_dir_ + "/" + fileName, VolumeLoader::Source::RAW };
				mi.setAction([this, src]{load(src); });
			} 
			else if (fileName.size() > 3 && fileName.substr(fileName.size() - 4, 4) == ".dcm") {
				MenuItem& mi = this->createItem("DICOM Files");
				VolumeLoader::Source src = { working_dir_, VolumeLoader::Source::DICOM_DIR };
				mi.setAction([this, src]{load(src); });
				break;
			}
		}
		entry = readdir(dir);
	}
	closedir(dir);
}

void DirectoryMenu::upDirectory()
{
	int i = working_dir_.rfind("/");
	std::string up = (i == 0) ? "/" : working_dir_.substr(0, i);
	directory(up);
}

void DirectoryMenu::load(const VolumeLoader::Source& source)
{
	if (on_load_) {
		on_load_(source);
	}

	//MainController::getInstance().setVolumeToLoad(source);
	//menus.pop();
	//MainController::getInstance().menuController().menu();
	//MainController::getInstance().showMenu(false);
}