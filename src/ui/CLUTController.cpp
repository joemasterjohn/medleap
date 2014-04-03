#include "CLUTController.h"

CLUTController::CLUTController()
{
    volumeRenderer = NULL;
    
    // start out with the default CLUT: black->white gradient
    cluts.push_back(CLUT());
    
    CLUT redToWhite;
    redToWhite.addColorStop(0.25f, cgl::Vec4(1, 0, 0, 1));
    cluts.push_back(redToWhite);
    
    CLUT test;
    test.addColorStop(0.2f, cgl::Vec4(0, 1, 0, 1));
    test.addColorStop(0.4f, cgl::Vec4(1, 1, 0, 1));
    test.addColorStop(0.6f, cgl::Vec4(1, 0, 0, 1));
    cluts.push_back(test);
    
    CLUT yellowToWhite;
    yellowToWhite.clearStops();
    yellowToWhite.addColorStop(0.0f, cgl::Vec4(1, 1, 0, 0));
    yellowToWhite.addColorStop(1.0f, cgl::Vec4(1, 1, 1, 1));
    cluts.push_back(yellowToWhite);
    
    CLUT monochrome;
    monochrome.clearStops();
    monochrome.addColorStop(0.0f, cgl::Vec4(1, 1, 1, 0));
    monochrome.addColorStop(1.0f, cgl::Vec4(1, 1, 1, 1));
    cluts.push_back(monochrome);
    
    renderer.setCLUT(&getActiveCLUT());
}

CLUTController::~CLUTController()
{
}

CLUTRenderer* CLUTController::getRenderer()
{
    return &renderer;
}

void CLUTController::setVolumeRenderer(VolumeRenderer* volumeRenderer)
{
    this->volumeRenderer = volumeRenderer;
    volumeRenderer->setCLUTTexture(renderer.getTexture());
}

void CLUTController::setSliceRenderer(SliceRenderer* sliceRenderer)
{
    this->sliceRenderer = sliceRenderer;
    sliceRenderer->setCLUTTexture(renderer.getTexture());
}

CLUT& CLUTController::getActiveCLUT()
{
    return cluts[activeCLUT];
}

bool CLUTController::keyboardInput(GLFWwindow* window, int key, int action, int mods)
{
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        activeCLUT = (activeCLUT + 1) % cluts.size();
        renderer.setCLUT(&getActiveCLUT());
    }
    
    return true;
}

bool CLUTController::mouseButton(GLFWwindow* window, int button, int action, int mods)
{
    // chekc for modifying active clut: reassign to vr
    return true;
}

bool CLUTController::mouseMotion(GLFWwindow* window, double x, double y)
{
    return true;
}