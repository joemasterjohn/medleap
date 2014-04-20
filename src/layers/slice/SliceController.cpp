#include "SliceController.h"

SliceController::SliceController()
{
    mouseLeftDrag = false;
}

SliceController::~SliceController()
{
}

SliceRenderer* SliceController::getRenderer()
{
    return &renderer;
}

void SliceController::setVolume(VolumeData* volume)
{
    this->volume = volume;
    renderer.setVolume(volume);
}

bool SliceController::keyboardInput(GLFWwindow* window, int key, int action, int mods)
{
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        int sliceIndex = (renderer.getCurrentSlice() + 1) % volume->getDepth();
        renderer.setCurrentSlice(sliceIndex);
    } else if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        int sliceIndex = renderer.getCurrentSlice() - 1;
        if (sliceIndex < 0)
            sliceIndex = volume->getDepth() - 1;
        renderer.setCurrentSlice(sliceIndex);
    }
    
    return true;
}

bool SliceController::mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mouseLeftDrag = true;
            glfwGetCursorPos(window, &mouseAnchorX, &mouseAnchorY);
            anchorSliceIndex = renderer.getCurrentSlice();
        } else if (action == GLFW_RELEASE) {
            mouseLeftDrag = false;
        }
    }
    
    return true;
}

bool SliceController::mouseMotion(GLFWwindow* window, double x, double y)
{
    if (!renderer.getViewport().contains(x,y))
        return true;
    
    // move through slices by dragging left and right
    if (mouseLeftDrag) {
        double dx = x - mouseAnchorX;
        int slice = static_cast<int>(anchorSliceIndex + dx * 0.1);
        if (slice < 0)
            slice = volume->getDepth() - ((-1*slice) % volume->getDepth());
        else
            slice = slice % volume->getDepth();
        
        renderer.setCurrentSlice(slice);
    }
    
    return true;
}

bool SliceController::scroll(GLFWwindow* window, double dx, double dy)
{
    return true;
}