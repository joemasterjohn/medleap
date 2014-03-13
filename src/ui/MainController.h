#ifndef __medleap__MainController__
#define __medleap__MainController__

#include "render/MainRenderer.h"
#include "ui/SliceController.h"
#include "ui/VolumeController.h"
#include "ui/VolumeInfoController.h"
#include <list>

/** Main class (singleton) that controls the UI events and owns the rendering. */
class MainController
{
public:
    ~MainController();
    void init();
    void startLoop();
    
    void setVolume(VolumeData* volume);
    
    void resize(int width, int height);
    void keyboardInput(GLFWwindow* window, int key, int action, int mods);
    void mouseButton(GLFWwindow* window, int button, int action, int mods);
    void mouseMotion(GLFWwindow* window, double x, double y);
    void scroll(GLFWwindow* window, double dx, double dy);
    
    static MainController& getInstance();
    
private:
    enum Mode
    {
        MODE_2D,
        MODE_3D
    };
    
    MainController();
    
    // copy constructor and assignment operators are not implemented as this class is a singleton
    MainController(const MainController& copy);
    MainController& operator=(const MainController& copy);
    
    void pushController(Controller* controller);
    
    void setMode(Mode mode);
    
    MainRenderer renderer;
    SliceController sliceController;
    VolumeController volumeController;
    VolumeInfoController volumeInfoController;
    std::list<Controller*> activeControllers;
    Mode mode;
    VolumeData* volume;
    
};

#endif /* defined(__medleap__MainController__) */
