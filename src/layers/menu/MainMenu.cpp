#include "MainMenu.h"
#include "DirectoryMenu.h"
#include "util/Config.h"

typedef std::shared_ptr<Menu> MenuPtr;

#define CONFIG_FILE_NAME ".medleap"
#define CONFIG_PARAM_WORKDIR "working_dir"

MainMenu::MainMenu(MenuManager* menuManager) : Menu("Main Menu")
{
#if defined(_WIN32)
    std::string homeDir = getenv("HOMEPATH");
#else
    std::string homeDir = getenv("HOME");
#endif
    
    Config cfg;
    std::string cfgFileName = homeDir + "/" + CONFIG_FILE_NAME;
    if (!cfg.load(cfgFileName)) {
        std::cout << "Creating default configuration: " << cfgFileName << std::endl;
        cfg.putValue(CONFIG_PARAM_WORKDIR, homeDir);
        cfg.save(cfgFileName);
    }

    std::string workingDir = cfg.getValue<std::string>(CONFIG_PARAM_WORKDIR);
    
    MenuItem& miLoad = createItem("Load");
    miLoad.setAction([=]{
        menuManager->push(MenuPtr(new DirectoryMenu(workingDir, menuManager)));
    });
    
    MenuItem& miRender = createItem("Render");
    MenuItem& miHide = createItem("Hide Menu");
    MenuItem& miExit = createItem("Exit");
}

MainMenu::~MainMenu()
{
}