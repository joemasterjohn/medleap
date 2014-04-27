#include "MainMenu.h"
#include "DirectoryMenu.h"
#include "main/MainConfig.h"
#include "main/MainController.h"

MainMenu::MainMenu() : Menu("Main Menu")
{
	// MainConfig cfg;
	//std::string workingDir = cfg.getValue<std::string>(MainConfig::WORKING_DIR);

	//MenuItem& miLoad = createItem("Load");
	////miLoad.setAction([=]{
	////    menuManager->push(MenuPtr(new DirectoryMenu(workingDir, *menuManager)));
	////});

	{
		MenuItem& item = createItem("Transfer");
		item.setAction([]{
			MainController& mc = MainController::getInstance();
			mc.focusLayer(&mc.transfer1DController());
			mc.showTransfer1D(true);
			mc.menuController().hideMenu();
		});
	}

	{
		MenuItem& item = createItem("2D/3D");
		item.setAction([]{
			MainController& mc = MainController::getInstance();
			if (mc.getMode() == MainController::MODE_2D) {
				mc.setMode(MainController::MODE_3D);
			} else {
				mc.setMode(MainController::MODE_2D);
			}
			mc.menuController().hideMenu();
		});
	}

	createItem("Edit");
	createItem("Load");

	{
		MenuItem& item = createItem("View");
		item.setAction([]{
			MainController& mc = MainController::getInstance();
			mc.focusLayer(&mc.volumeController());
			mc.menuController().hideMenu();
		});
	}
}

MainMenu::~MainMenu()
{
}