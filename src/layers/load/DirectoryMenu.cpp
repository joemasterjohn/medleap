#include "DirectoryMenu.h"
#include "main/MainController.h"

#if defined(_WIN32)
#include "util/dirent.h"
#define DELIM "\\"
#else
#define DELIM "/"
#include <dirent.h>
#endif
#include <errno.h>

static enum class FileType
{
	other,
	readable_dir,
	dcm,
	file,
	raw
};

static FileType fileType(const std::string& wd, struct dirent* entry)
{
	if (entry->d_type == DT_DIR) {
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
			return FileType::other;
		}

		std::string full_name = wd + DELIM + std::string{ entry->d_name };
		DIR* dir = opendir(full_name.c_str());
		FileType type = FileType::other;
		if (dir) {
			type = FileType::readable_dir;
		}

		closedir(dir);
		return type;
	} else {
		std::string name{ entry->d_name };
		if (name.size() > 3 && name.substr(name.size() - 4, 4) == ".raw") {
			return FileType::raw;
		} else if (name.size() > 3 && name.substr(name.size() - 4, 4) == ".dcm") {
			return FileType::dcm;
		}

		return FileType::file;
	}
}

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
		FileType type = fileType(working_dir_, entry);
		std::string subdir{ entry->d_name };
		std::string full_subdir{ working_dir_ + DELIM + subdir };

		if (type == FileType::readable_dir) {
			createItem(subdir, [this, full_subdir]{ directory(full_subdir); });
		} else if (type == FileType::raw) {
			createItem(subdir, [this, full_subdir]{ load({ full_subdir, VolumeLoader::Source::RAW }); });
		} else if (type == FileType::dcm) {
			createItem(subdir, [this]{ load({ working_dir_, VolumeLoader::Source::DICOM_DIR }); });
			break;
		}
		entry = readdir(dir);
	}
	closedir(dir);
}

void DirectoryMenu::upDirectory()
{
	int i = working_dir_.rfind(DELIM);
	std::string up = (i == 0) ? DELIM : working_dir_.substr(0, i);
	directory(up);
}

void DirectoryMenu::load(const VolumeLoader::Source& source)
{
	if (on_load_) {
		on_load_(source);
	}
}