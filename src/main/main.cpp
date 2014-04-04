#include "gl/glew.h"
#include "MainController.h"
#include "data/VolumeLoader.h"
#include <iostream>

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "provide dicom directory as argument" << std::endl;
        return 0;
    }
    
    VolumeLoader loader;
    VolumeData* volume = loader.load(argv[1]);
    if (!volume) {
        std::cout << "could not find volume " << argv[1] << std::endl;
        return 0;
    }
    
    MainController& controller = MainController::getInstance();
    controller.init();
    controller.setVolume(volume);
    controller.startLoop();
    
    return 0;
}
