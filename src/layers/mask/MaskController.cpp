#include "MaskController.h"
#include "main/MainController.h"
#include "gl/math/Math.h"

using namespace gl;
using namespace std;
using namespace Leap;

MaskController::MaskController() : mask_volume_(new SphereMask(Sphere(Vec3(0.0f), 0.1f)))
{
	VolumeController& vc = MainController::getInstance().volumeController();
	vc.mask_cursor_ = Sphere(Vec3(0.0f), 0.1f);
}

bool MaskController::leapInput(const Leap::Controller& controller, const Leap::Frame& frame)
{
	tracker_.update(controller);
	if (tracker_.tracking()) {
		VolumeController& vc = MainController::getInstance().volumeController();
		Vector v2 = tracker_.palmPosCurrent();
		v2 -= controller.frame(1).hand(tracker_.handCurrent().id()).palmPosition();

		Mat4 eye2world = vc.getCamera().getView().inverse();
		Vec4 v = eye2world * Vec4(v2.x, v2.y, v2.z, 0.0f);

		cursor_ += v / 100.0f;

		vc.mask_cursor_.center() = cursor_;
		//tool_position_ = 
		scrubVoxels();
		vc.markDirty();
	}

	return false;
}

void MaskController::scrubVoxels()
{
	//{
	//	static int x = 0;
	//	vector<GLubyte> dat;
	//	dat.resize(64, (GLubyte)255);
	//	maskTexture.bind();
	//	glTexSubImage3D(GL_TEXTURE_3D, 0, x, 0, 0, 4, 4, 4, GL_RED, GL_UNSIGNED_BYTE, &dat[0]);
	//	x++;
	//	std::cout << x << std::endl;
	//	if (x == volume->getWidth())
	//		x = 0;
	//}
}