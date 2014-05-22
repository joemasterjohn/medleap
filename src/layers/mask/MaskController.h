#ifndef __medleap_MaskController__
#define __medleap_MaskController__

#include "layers/Controller.h"
#include "leap/VPose.h"
#include "gl/math/Math.h"
#include "MaskVolume.h"
#include "util/History.h"
#include "leap/PoseTracker.h"
#include "layers/volume/LeapCameraControl.h"

class MaskController : public Controller
{
public:
	MaskController();

	void gainFocus() override;
	void loseFocus() override;
	bool modal() const override;
	bool leapInput(const Leap::Controller& controller, const Leap::Frame& frame) override;

private:
	PoseTracker poses_;
	LeapCameraControl cam_control_;
	std::unique_ptr<MaskVolume> mask_volume_;
	History<MaskVolume::Edit, 10> edits_;
	bool editing_;

	void moveCursor();
	void applyEdit();
};

#endif // __medleap_MaskController__
