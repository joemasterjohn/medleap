#ifndef __medleap__MainController__
#define __medleap__MainController__

#include "render/MainRenderer.h"
#include "ui/SliceController.h"
#include "ui/VolumeController.h"
#include "ui/VolumeInfoController.h"
#include "ui/HistogramController.h"
#include "ui/CLUTController.h"
#include <list>
#include "util/TextRenderer.h"

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
    
    TextRenderer& getText();
    
    static MainController& getInstance();
    
private:
    enum Mode
    {
        MODE_2D,
        MODE_3D
    };
    
    class Docking
    {
    public:
        enum Position { NONE, LEFT, RIGHT, BOTTOM, TOP };
        Position position;
        double percent;
        Docking(Position position, double percent) : position(position), percent(percent) {}
    };
    
    MainController();
    
    // copy constructor and assignment operators are not implemented as this class is a singleton
    MainController(const MainController& copy);
    MainController& operator=(const MainController& copy);
    
    void pushController(Controller* controller);
    void pushController(Controller* controller, Docking docking);

    void setMode(Mode mode);
    
    void toggleHistogram();
    
    MainRenderer renderer;
    SliceController sliceController;
    VolumeController volumeController;
    VolumeInfoController volumeInfoController;
    HistogramController histogramController;
    CLUTController clutController;
    std::list<Controller*> activeControllers;
    Mode mode;
    VolumeData* volume;
    bool showHistogram;
    TextRenderer text;
};

#endif /* defined(__medleap__MainController__) */
