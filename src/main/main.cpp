#include "gl/glew.h"
#include "MainController.h"
#include <iostream>

int main(int argc, char** argv)
{
    MainController& controller = MainController::getInstance();
    controller.init();
    
    if (argc > 1) {
        controller.setVolumeToLoad(argv[1]);
    }
    
    controller.startLoop();
    
    return 0;
}