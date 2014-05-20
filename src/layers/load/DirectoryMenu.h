#ifndef __medleap_DirectoryMenu__
#define __medleap_DirectoryMenu__

#include "layers/menu/Menu.h"
#include "data/VolumeLoader.h"

class DirectoryMenu : public Menu
{
public:
	DirectoryMenu();
        
	void directory(const std::string& dir);
	void upDirectory();

private:
    std::string working_dir_;

	void load(const VolumeLoader::Source& source);
};

#endif // __medleap_DirectoryMenu__