#include "MaskController.h"
#include "main/MainController.h"
#include "gl/math/Math.h"

using namespace gl;
using namespace std;
using namespace Leap;

MaskController::MaskController()
{
}

bool MaskController::leapInput(const Leap::Controller& controller, const Leap::Frame& frame)
{
	tracker_.update(controller);
	if (tracker_.tracking()) {
		//tool_position_ = 
		scrubVoxels();
	}

	return false;
}

void MaskController::scrubVoxels()
{

}