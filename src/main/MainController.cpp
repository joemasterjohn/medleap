#include "MainController.h"
#include "data/VolumeLoader.h"
#include <chrono>

using namespace std;

void keyboardCB(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    MainController::getInstance().keyboardInput(window, key, action, mods);
}

void resizeCB(GLFWwindow* window, int width, int height)
{
    MainController::getInstance().resize(width, height);
}

void mouseCB(GLFWwindow* window, int button, int action, int mods)
{
    MainController::getInstance().mouseButton(window, button, action, mods);
}

void cursorCB(GLFWwindow* window, double x, double y)
{
    MainController::getInstance().mouseMotion(window, x, y);
}

void scrollCB(GLFWwindow* window, double dx, double dy)
{
    MainController::getInstance().scroll(window, dx, dy);
}

TextRenderer& MainController::getText()
{
    return text;
}

MainController& MainController::getInstance()
{
    static MainController controller;
    return controller;
}

MainController::MainController() :
    volume(NULL)
{
    mode = MODE_3D;
    showHistogram = true;
	menuOn = false;
}

MainController::~MainController()
{
}

void MainController::init()
{
    bool status = renderer.init(800, 800, "MedLeap");
    if (!status) {
        cout << "Failed to initialize renderer." << endl;
        return;
    }

    glfwSetFramebufferSizeCallback(renderer.getWindow(), resizeCB);
	glfwSetKeyCallback(renderer.getWindow(), keyboardCB);
	glfwSetMouseButtonCallback(renderer.getWindow(), mouseCB);
    glfwSetCursorPosCallback(renderer.getWindow(), cursorCB);
    glfwSetScrollCallback(renderer.getWindow(), scrollCB);
    
    text.loadFont("menlo14");
    
    sliceController.getRenderer()->init();
    volumeController.getRenderer()->init();
    volumeInfoController.getRenderer()->init();
    histogramController.getRenderer()->init();
	menuController.getRenderer()->init();
    
    volumeInfoController.getRenderer()->setVolumeRenderer(volumeController.getRenderer());
    volumeInfoController.getRenderer()->setSliceRenderer(sliceController.getRenderer());
    histogramController.setVolumeRenderer(volumeController.getRenderer());
    histogramController.setSliceRenderer(sliceController.getRenderer());
}

void MainController::setMode(MainController::Mode mode)
{
    this->mode = mode;
    switch (mode) {
        case MODE_2D:
            renderer.clearLayers();
            activeControllers.clear();
            pushController(&sliceController);
            pushController(&volumeInfoController);
            if (showHistogram)
                pushController(&histogramController, Docking(Docking::BOTTOM, 0.2));
            break;
        case MODE_3D:
            renderer.clearLayers();
            activeControllers.clear();
            pushController(&volumeController);
            pushController(&volumeInfoController);
            if (showHistogram)
                pushController(&histogramController, Docking(Docking::BOTTOM, 0.2));
            break;
    }
}

MainController::Mode MainController::getMode()
{
    return mode;
}

void MainController::setVolume(VolumeData* volume)
{
    if (this->volume == volume)
        return;
    
    if (this->volume != NULL)
        delete this->volume;
    else
        setMode(mode);
    
    this->volume = volume;
    sliceController.setVolume(volume);
    volumeController.setVolume(volume);
    volumeInfoController.setVolume(volume);
    histogramController.setVolume(volume);
}

void MainController::setVolumeToLoad(const std::string& directory)
{
    loader.setSource(directory);
}

void MainController::startLoop()
{
    static double f = 0;

    while (!glfwWindowShouldClose(renderer.getWindow())) {

		if (loader.getState() == VolumeLoader::FINISHED) {
            setVolume(loader.getVolume());
            volumeController.getRenderer()->markDirty();
        }

		update();
        renderer.draw();

		if (loader.getState() == VolumeLoader::LOADING) {
			// draw load screen PUSH and POP layer
			GLclampf c = static_cast<GLclampf>((std::sin(f += 0.01) * 0.5 + 0.5) * 0.5 + 0.5);
			//glClearColor(c, c, c, 1);
			glViewport(0, 0, renderer.getWidth(), renderer.getHeight());
			getText().setColor(1, c, c);
			getText().begin(renderer.getWidth(), renderer.getHeight());
			getText().add(string("Loading"), renderer.getWidth() / 2, renderer.getHeight() / 2, TextRenderer::CENTER, TextRenderer::CENTER);
			getText().add(loader.getStateMessage(), renderer.getWidth() / 2, renderer.getHeight() / 2 - 36, TextRenderer::CENTER, TextRenderer::CENTER);
			getText().end();
		}

		glfwSwapBuffers(renderer.getWindow());
    }
    glfwTerminate();
}

void MainController::showMenu(bool show)
{
	if ((show && menuOn) || (!show && !menuOn))
		return;

	if (show) {
		pushController(&menuController);
		menuOn = true;
	}
	else {
		popController();
		menuController.getMenuManager().reset();
		menuOn = false;
	}
}

void MainController::update()
{
    static auto prevTime = chrono::high_resolution_clock::now();
	
	auto curTime = chrono::high_resolution_clock::now();
	chrono::milliseconds elapsed = chrono::duration_cast<chrono::milliseconds>(curTime - prevTime);
	prevTime = curTime;

	glfwPollEvents();

	for (Controller* c : activeControllers) {
		c->update(elapsed);
	}

	if (leapController.isConnected()) {
		Leap::Frame currentFrame = leapController.frame();

		if (!menuOn) {
			Leap::GestureList gestures = currentFrame.gestures();
			for (const Leap::Gesture& g : gestures) {
				if (g.type() == Leap::Gesture::TYPE_CIRCLE && currentFrame.tools().count() == 0) {
					Leap::Vector tp = Leap::CircleGesture(g).center();
					menuController.setLeapCenter(Vec2(tp.x, tp.y));
					showMenu(true);
				}
			}
		}

		for (Controller* c : activeControllers) {
			bool passThrough = c->leapInput(leapController, currentFrame);
			if (!passThrough) {
				break;
			}
		}
	}
}

void MainController::toggleHistogram()
{
    showHistogram = !showHistogram;
    setMode(mode);
}

void MainController::keyboardInput(GLFWwindow *window, int key, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		showMenu(!menuOn);
	}

	if (!menuOn) {
		if (key == GLFW_KEY_M && action == GLFW_PRESS) {
			setMode((mode == MODE_2D) ? MODE_3D : MODE_2D);
		}

		if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
			volume->setNextWindow();
		}

		if (key == GLFW_KEY_H && action == GLFW_PRESS)
			toggleHistogram();
	}
    
	for (Controller* c : activeControllers) {
		bool passThrough = c->keyboardInput(window, key, action, mods);
		if (!passThrough) {
			break;
		}
	}
}

void MainController::resize(int width, int height)
{
    renderer.resize(width, height);
    for (Controller* c : activeControllers) {
        if (c->getRenderer())
            c->getRenderer()->resize(width, height);
    }
}

void MainController::mouseButton(GLFWwindow *window, int button, int action, int mods)
{
	for (Controller* c : activeControllers) {
		bool passThrough = c->mouseButton(window, button, action, mods);
		if (!passThrough)
			break;
	}
}

void MainController::mouseMotion(GLFWwindow *window, double x, double y)
{
    // convert y to bottom up
    y = renderer.getHeight() - y - 1;
    for (Controller* c : activeControllers) {
        bool passThrough = c->mouseMotion(window, x, y);
		if (!passThrough)
			break;
    }
}

void MainController::scroll(GLFWwindow *window, double dx, double dy)
{
	for (Controller* c : activeControllers) {
		bool passThrough = c->scroll(window, dx, dy);
		if (!passThrough)
			break;
	}
}

void MainController::popController()
{
	Controller* c = activeControllers.front();
	if (c->getRenderer())
		renderer.popLayer();
	activeControllers.pop_front();

	chooseTrackedGestures();
}

void MainController::pushController(Controller* controller)
{
    Docking docking(MainController::Docking::NONE, 0);
    pushController(controller, docking);
}

void MainController::pushController(Controller* controller, MainController::Docking docking)
{
    activeControllers.push_front(controller);
    if (controller->getRenderer()) {
        
        switch (docking.position)
        {
            case MainController::Docking::LEFT:
                renderer.dockLeft(controller->getRenderer(), docking.percent);
                break;
            case MainController::Docking::RIGHT:
                renderer.dockRight(controller->getRenderer(), docking.percent);
                break;
            case MainController::Docking::BOTTOM:
                renderer.dockBottom(controller->getRenderer(), docking.percent);
                break;
            case MainController::Docking::TOP:
                renderer.dockTop(controller->getRenderer(), docking.percent);
                break;
            default:
                renderer.pushLayer(controller->getRenderer());
        }
    }
	chooseTrackedGestures();
}

void MainController::chooseTrackedGestures()
{
	// MainController uses the CIRCLE gesutre to toggle the menu
	leapController.enableGesture(Leap::Gesture::TYPE_CIRCLE, true);

	leapController.enableGesture(Leap::Gesture::TYPE_KEY_TAP, false);
	leapController.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP, false);
	leapController.enableGesture(Leap::Gesture::TYPE_SWIPE, false);

	// enable any requested gestures from all controllers
	set<Leap::Gesture::Type> allRequired;
	for (Controller* c : activeControllers) {
		for (const Leap::Gesture::Type t : c->requiredGestures())
			leapController.enableGesture(t, true);
	}
}