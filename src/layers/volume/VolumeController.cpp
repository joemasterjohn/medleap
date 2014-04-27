#include "VolumeController.h"

using namespace gl;

VolumeController::VolumeController()
{
    mouseDragLeftButton = false;
    mouseDragRightButton = false;
}

VolumeController::~VolumeController()
{
}

VolumeRenderer* VolumeController::getRenderer()
{
    return &renderer;
}

void VolumeController::setVolume(VolumeData* volume)
{
    this->volume = volume;
    renderer.setVolume(volume);
}

bool VolumeController::keyboardInput(GLFWwindow* window, int key, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_1:
		if (action == GLFW_PRESS)
			renderer.setOpacityScale(renderer.getOpacityScale() - 0.1f);
		break;
	case GLFW_KEY_2:
		if (action == GLFW_PRESS)
			renderer.setOpacityScale(renderer.getOpacityScale() + 0.1f);
		break;
	case GLFW_KEY_V:
		if (action == GLFW_PRESS)
			renderer.cycleMode();
		break;
	case GLFW_KEY_L:
		if (action == GLFW_PRESS)
			renderer.toggleShading();
		break;
	case GLFW_KEY_J:
		if (action == GLFW_PRESS) {
			renderer.useJitter = !renderer.useJitter;
			renderer.markDirty();
		}
		break;
	}

    return true;
}

bool VolumeController::mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        mouseDragLeftButton = action == GLFW_PRESS;
        if (mouseDragLeftButton) {
            dragStartView = renderer.getCamera().getView();
        }
    }
    
//    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
//        mouseDragRightButton = action == GLFW_PRESS;
//        if (mouseDragRightButton) {
//            dragStartView = renderer.getCamera().getView();
//        }
//        renderer.setMoving(mouseDragRightButton);
//    }
    
    return true;
}

bool VolumeController::mouseMotion(GLFWwindow* window, double x, double y)
{
    if (!renderer.getViewport().contains(x, y))
        return true;

    
    if (mouseDragLeftButton) {
        double dx = x - dragStartX;
        double dy = y - dragStartY;
        double pitch = dy * 0.01;
        double yaw = dx * 0.01;
        
        Mat4 m1 = gl::rotation(static_cast<float>(pitch), dragStartView.row(0));
        Mat4 m2 = gl::rotation(static_cast<float>(yaw), dragStartView.row(1));
        renderer.getCamera().setView(dragStartView * m1 * m2);
        renderer.markDirty();
    } else if (mouseDragRightButton) {
//        double dx = x - dragStartX;
//        double dy = y - dragStartY;
//        
//        Mat4 tm = translation(dx*0.002, dy*0.002, 0);
//        renderer.getCamera().setView(tm * dragStartView);
//        
    } else {
        dragStartX = x;
        dragStartY = y;
    }

    return true;
}

bool VolumeController::scroll(GLFWwindow* window, double dx, double dy)
{
    if (!mouseDragLeftButton) {
        renderer.getCamera().translateBackward(static_cast<float>(dy * 0.2f));
        renderer.markDirty();
    }
    return true;
}

bool VolumeController::leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame)
{
	Leap::FingerList fingers = currentFrame.fingers();


	static Vec3 toolStart;
	static int framesSlow = 0;
	static Vec3 oldCursorPos;

	if (fingers.count() > 0) {
		Leap::Finger p = fingers.frontmost();

		if (!renderer.cursorActive && p.tipVelocity().magnitude() < 100) {
			framesSlow++;
			if (framesSlow > 40) {
				renderer.cursorActive = true;
				toolStart.x = p.tipPosition().x;
				toolStart.y = p.tipPosition().y;
				toolStart.z = p.tipPosition().z;
				oldCursorPos = renderer.cursor3D;
				framesSlow = 0;
			}
		}

		if (renderer.cursorActive) {

			if (p.tipVelocity().magnitude() > 1200) {
				framesSlow = 0;
				renderer.cursorActive = false;

			}
			else {
	
				Vec3 d = Vec3(p.tipPosition().x, p.tipPosition().y, p.tipPosition().z) - toolStart;

				Camera& cam = renderer.getCamera();
				Vec3 offset;
				offset += cam.getRight() * d.x * 0.003;
				offset += cam.getUp() * d.y* 0.003;
				offset += cam.getForward() * -d.z* 0.003;

				renderer.cursor3D = oldCursorPos + offset;
				renderer.markDirty();
			}
		}
	}

	if (currentFrame.fingers().count() == 0) {
		framesSlow = 0;
		renderer.cursorActive = false;
	}

	return true;
}