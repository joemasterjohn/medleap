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
		MenuItem& item = createItem("Test");
		item.setAction([]{
			MainController& mc = MainController::getInstance();
			mc.menuController().hideMenu();
			mc.pickColor(ColorRGB(1, 0, 0, 1), [](const Color& color){
				std::cout << color.rgb().vec3() << std::endl;
			});
		});
	}

	createItem("Load");

	{
		MenuItem& item = createItem("View 3D");
		item.setAction([]{
			MainController& mc = MainController::getInstance();
			mc.showTransfer1D(false);
			mc.setMode(MainController::MODE_3D);
			mc.focusLayer(&mc.volumeController());
			mc.menuController().hideMenu();
		});
	}


	{
		MenuItem& item = createItem("View 2D");
		item.setAction([]{
			MainController& mc = MainController::getInstance();
			mc.showTransfer1D(false);
			mc.setMode(MainController::MODE_2D);
			mc.focusLayer(&mc.sliceController());
			mc.menuController().hideMenu();
		});
	}
}

MainMenu::~MainMenu()
{
}