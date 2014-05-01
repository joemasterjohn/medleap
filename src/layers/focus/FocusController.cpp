#include "FocusController.h"
#include "main/MainController.h"
#include "gl/math/Math.h"

using namespace gl;
using namespace std;
using namespace Leap;

FocusController::FocusController()
{
}

bool FocusController::leapInput(const Leap::Controller& controller, const Leap::Frame& frame)
{
	//Leap::FingerList fingers = currentFrame.fingers();


	//static Vec3 toolStart;
	//static int framesSlow = 0;
	//static Vec3 oldCursorPos;

	//if (fingers.count() > 0) {
	//	Leap::Finger p = fingers.frontmost();

	//	if (!cursorActive && p.tipVelocity().magnitude() < 100) {
	//		framesSlow++;
	//		if (framesSlow > 40) {
	//			cursorActive = true;
	//			toolStart.x = p.tipPosition().x;
	//			toolStart.y = p.tipPosition().y;
	//			toolStart.z = p.tipPosition().z;
	//			oldCursorPos = cursor3D;
	//			framesSlow = 0;
	//		}
	//	}

	//	if (cursorActive) {

	//		if (p.tipVelocity().magnitude() > 1200) {
	//			framesSlow = 0;
	//			cursorActive = false;

	//		} else {

	//			Vec3 d = Vec3(p.tipPosition().x, p.tipPosition().y, p.tipPosition().z) - toolStart;

	//			Camera& cam = camera;
	//			Vec3 offset;
	//			offset += cam.getRight() * d.x * 0.003;
	//			offset += cam.getUp() * d.y* 0.003;
	//			offset += cam.getForward() * -d.z* 0.003;

	//			cursor3D = oldCursorPos + offset;
	//			markDirty();
	//		}
	//	}
	//}

	//if (currentFrame.fingers().count() == 0) {
	//	framesSlow = 0;
	//	cursorActive = false;
	//}

	return false;
}