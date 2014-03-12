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
    setMode(MODE_2D);
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
            renderer.pushLayer(controller2D.getRenderLayer());
            break;
        case MODE_3D:
            renderer.clearLayers();
            renderer.pushLayer(controller3D.getRenderLayer());
            break;
    }
}

void MainController::setVolume(VolumeData* volume)
{
    if (this->volume == volume)
        return;
    
    if (this->volume == NULL) {
        controller2D.getRenderLayer()->init();
        controller3D.getRenderLayer()->init();
    } else {
        delete this->volume;
    }
    
    this->volume = volume;
    controller2D.setVolume(volume);
    controller3D.setVolume(volume);
    
    controller2D.getRenderLayer()->resize(renderer.getWidth(), renderer.getHeight());
    controller3D.getRenderLayer()->resize(renderer.getWidth(), renderer.getHeight());
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

    
    // TODO go through controllers and process
}

void MainController::resize(int width, int height)
{
    renderer.resize(width, height);
    controller2D.getRenderLayer()->resize(width, height);
    controller3D.getRenderLayer()->resize(width, height);
}

void MainController::mouseButton(GLFWwindow *window, int button, int action, int mods)
{
    // TODO go through controllers and process
}

void MainController::mouseMotion(GLFWwindow *window, double x, double y)
{
    // TODO go through controllers and process
}

void MainController::scroll(GLFWwindow *window, double dx, double dy)
{
    // TODO go through controllers and process
}