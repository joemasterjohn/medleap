#include "gl/glew.h"
#include "MainController.h"
#include <iostream>


int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "provide dicom directory as argument" << std::endl;
        return 0;
    }
    
    MainController& controller = MainController::getInstance();
    controller.init();
    controller.setVolumeToLoad(argv[1]);
    controller.startLoop();
    
    return 0;
}