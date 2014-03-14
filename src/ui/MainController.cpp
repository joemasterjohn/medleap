#include "MainController.h"

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



MainController& MainController::getInstance()
{
    static MainController controller;
    return controller;
}

MainController::MainController() :
    volume(NULL)
{
    mode = MODE_2D;
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
            break;
        case MODE_3D:
            renderer.clearLayers();
            activeControllers.clear();
            pushController(&volumeController);
            pushController(&volumeInfoController);
            break;
    }
}

void MainController::setVolume(VolumeData* volume)
{
    if (this->volume == volume)
        return;
    
    if (this->volume == NULL) {
        sliceController.getRenderer()->init();
        volumeController.getRenderer()->init();
        volumeInfoController.getRenderer()->init();
    } else {
        delete this->volume;
    }
    
    this->volume = volume;
    sliceController.setVolume(volume);
    volumeController.setVolume(volume);
    volumeInfoController.setVolume(volume);
    histogramController.setVolume(volume);
    
    setMode(mode);
}

void MainController::startLoop()
{
    while (!glfwWindowShouldClose(renderer.getWindow())) {
        renderer.draw();
        glfwPollEvents();
    }
    glfwTerminate();
}

void MainController::keyboardInput(GLFWwindow *window, int key, int action, int mods)
{
    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        setMode((mode == MODE_2D) ? MODE_3D : MODE_2D);
    }
    
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        volume->setNextWindow();
    }

    for (Controller* c : activeControllers)
        c->keyboardInput(window, key, action, mods);
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
    for (Controller* c : activeControllers)
        c->mouseButton(window, button, action, mods);
}

void MainController::mouseMotion(GLFWwindow *window, double x, double y)
{
    for (Controller* c : activeControllers)
        c->mouseMotion(window, x, y);
}

void MainController::scroll(GLFWwindow *window, double dx, double dy)
{
    for (Controller* c : activeControllers)
        c->scroll(window, dx, dy);
}

void MainController::pushController(Controller* controller)
{
    activeControllers.push_front(controller);
    if (controller->getRenderer()) {
        renderer.pushLayer(controller->getRenderer());
        controller->getRenderer()->resize(renderer.getWidth(), renderer.getHeight());
    }
}