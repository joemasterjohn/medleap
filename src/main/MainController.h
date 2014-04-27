#ifndef __medleap__MainController__
#define __medleap__MainController__

#include "MainRenderer.h"
#include "layers/slice/SliceController.h"
#include "layers/volume/VolumeController.h"
#include "layers/volume_info/VolumeInfoController.h"
#include "layers/transfer_1D/Transfer1DController.h"
#include "layers/menu/MenuController.h"
#include "layers/orientation/OrientationController.h"
#include "layers/color_pick/ColorPickController.h"
#include "util/TextRenderer.h"
#include "data/VolumeLoader.h"
#include "Leap.h"
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

	class Docking
	{
	public:
		enum Position { NONE, LEFT, RIGHT, BOTTOM, TOP };
		Position position;
		double percent;
		Docking(Position position, double percent) : position(position), percent(percent) {}
	};

    ~MainController();
    void init(GLFWwindow* window);
    void startLoop();
    void setVolume(VolumeData* volume);
    void resize(int width, int height);
    void keyboardInput(GLFWwindow* window, int key, int action, int mods);
    void mouseButton(GLFWwindow* window, int button, int action, int mods);
    void mouseMotion(GLFWwindow* window, double x, double y);
    void scroll(GLFWwindow* window, double dx, double dy);
    TextRenderer& getText();
    Mode getMode();
	void setVolumeToLoad(const VolumeLoader::Source& source);
    static MainController& getInstance();
	const MainRenderer& getRenderer() const;

	VolumeController& volumeController() { return volumeController_; }
	Transfer1DController& transfer1DController() { return histogramController; }
	MenuController& menuController() { return menuController_; }

	void pickColor(const Color& initialColor, std::function<void(const Color&)> callback);

	void popController();
	void pushController(Controller* controller);
	void pushController(Controller* controller, Docking docking);

	void showTransfer1D(bool show);

	Controller* focusLayer();
	void focusLayer(Controller* controller);
	void setMode(Mode mode);

private:    
    MainController();

	// no copying
    MainController(const MainController& copy) = delete;
    MainController& operator=(const MainController& copy) = delete;
	
	void update();

    void toggleHistogram();
	void chooseTrackedGestures();

	std::stack<Controller*> focus_stack_;
	GLFWwindow* window;
	Leap::Controller leapController;
    MainRenderer renderer;
    SliceController sliceController;
	VolumeController volumeController_;
    VolumeInfoController volumeInfoController;
    Transfer1DController histogramController;
	MenuController menuController_;
	ColorPickController colorPickController;
	OrientationController orientationController;
    std::list<Controller*> activeControllers;
    Mode mode;
    VolumeData* volume;
    bool showHistogram;
    TextRenderer text;
    VolumeLoader loader;
	int width;
	int height;
	double mMouseX;
	double mMouseY;

	void pushFocus(Controller* focus);
	void popFocus();
};

#endif /* defined(__medleap__MainController__) */
