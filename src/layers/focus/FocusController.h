#ifndef __medleap_FocusController__
#define __medleap_FocusController__

#include "layers/Controller.h"
#include "leap/PointPose.h"

class FocusController : public Controller
{
public:
	FocusController();

	bool leapInput(const Leap::Controller& controller, const Leap::Frame& frame) override;
	void gainFocus() override;
	void loseFocus() override;

private:
	PointPose point_pose_;
};

#endif // __medleap_FocusController__
