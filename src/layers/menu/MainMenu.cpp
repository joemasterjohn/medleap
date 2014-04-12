#include "MainMenu.h"
#include "DirectoryMenu.h"
#include "main/MainConfig.h"
#include "main/MainController.h"

typedef std::shared_ptr<Menu> MenuPtr;

MainMenu::MainMenu(MenuManager* menuManager) : Menu("Main Menu")
{
    MainConfig cfg;
    std::string workingDir = cfg.getValue<std::string>(MainConfig::WORKING_DIR);
    
    MenuItem& miLoad = createItem("Load");
    miLoad.setAction([=]{
        menuManager->push(MenuPtr(new DirectoryMenu(workingDir, *menuManager)));
    });
    
    MenuItem& miRender = createItem("Render");

    MenuItem& miHide = createItem("Hide Menu");
	miHide.setAction([]{ MainController::getInstance().showMenu(false); });

    MenuItem& miExit = createItem("Exit");
}

MainMenu::~MainMenu()
{
}