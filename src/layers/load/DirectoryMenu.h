#ifndef __medleap_DirectoryMenu__
#define __medleap_DirectoryMenu__

#include "layers/menu/Menu.h"
#include "data/VolumeLoader.h"
#include <functional>

class DirectoryMenu : public Menu
{
public:
	DirectoryMenu();
	void directory(const std::string& dir);
	void upDirectory();

	void onLoad(std::function<void(const VolumeLoader::Source&)> on_load) { on_load_ = on_load; }

private:
	std::function<void(const VolumeLoader::Source&)> on_load_;
    std::string working_dir_;

	void load(const VolumeLoader::Source& source);
};

#endif // __medleap_DirectoryMenu__