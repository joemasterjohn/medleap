#include "gl/glew.h"
#include "MainController.h"
#include <iostream>

int main(int argc, char** argv)
{
    MainController& controller = MainController::getInstance();
    controller.init();
    
    if (argc > 1) {
		VolumeLoader::Source src = { argv[1], VolumeLoader::Source::DICOM_DIR };
        controller.setVolumeToLoad(src);
    }
    
    controller.startLoop();
    
    return 0;
}