#include "MainMenu.h"
#include "DirectoryMenu.h"
#include "main/MainConfig.h"
#include "main/MainController.h"

MainMenu::MainMenu() : Menu("Main Menu")
{
	MenuItem& mi_transfer = createItem("Transfer");
	mi_transfer.setAction([]{
		MainController& mc = MainController::getInstance();
		mc.focusLayer(&mc.transfer1DController());
		mc.showTransfer1D(true);
		mc.menuController().hideMenu();
	});

	MenuItem& mi_clip = createItem("Clip");
	mi_clip.setAction([]{
		MainController& mc = MainController::getInstance();
		mc.focusLayer(&mc.clipController());
		mc.menuController().hideMenu();
	});

	MenuItem& mi_focus = createItem("Focus");
	mi_focus.setAction([]{
		MainController& mc = MainController::getInstance();
		mc.focusLayer(&mc.focusController());
		mc.menuController().hideMenu();
	});

	MenuItem& mi_mask = createItem("Mask");
	mi_mask.setAction([]{
		MainController& mc = MainController::getInstance();
		mc.focusLayer(&mc.maskController());
		mc.menuController().hideMenu();
	});

	MenuItem& mi_view3d = createItem("View 3D");
	mi_view3d.setAction([]{
		MainController& mc = MainController::getInstance();
		mc.showTransfer1D(false);
		mc.setMode(MainController::MODE_3D);
		mc.focusLayer(&mc.volumeController());
		mc.menuController().hideMenu();
	});

	MenuItem& mi_view2d = createItem("View 2D");
	mi_view2d.setAction([]{
		MainController& mc = MainController::getInstance();
		mc.showTransfer1D(false);
		mc.setMode(MainController::MODE_2D);
		mc.focusLayer(&mc.sliceController());
		mc.menuController().hideMenu();
	});
}

MainMenu::~MainMenu()
{
}