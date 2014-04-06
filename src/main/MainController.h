#ifndef __medleap__MainController__
#define __medleap__MainController__

#include "MainRenderer.h"
#include "layers/slice/SliceController.h"
#include "layers/volume/VolumeController.h"
#include "layers/volume_info/VolumeInfoController.h"
#include "layers/transfer_1D/Transfer1DController.h"
#include "layers/menu/MenuController.h"
#include "util/TextRenderer.h"
#include "data/VolumeLoader.h"
#include <list>

/** Main class (singleton) that controls the UI events and owns the rendering. */
class MainController
{
public:
    enum Mode
    {
        MODE_2D,
        MODE_3D
    };
    
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
    Mode getMode();
	void setVolumeToLoad(const char* directory);
    static MainController& getInstance();
    
private:
    
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

	void popController();
    void pushController(Controller* controller);
    void pushController(Controller* controller, Docking docking);
    void setMode(Mode mode);
    void toggleHistogram();
    
    MainRenderer renderer;
    SliceController sliceController;
    VolumeController volumeController;
    VolumeInfoController volumeInfoController;
    Transfer1DController histogramController;
	MenuController menuController;
    std::list<Controller*> activeControllers;
    Mode mode;
    VolumeData* volume;
    bool showHistogram;
    TextRenderer text;
    VolumeLoader loader;
	bool menuOn;
};

#endif /* defined(__medleap__MainController__) */
