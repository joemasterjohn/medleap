#ifndef __medleap_ClipController__
#define __medleap_ClipController__

#include "layers/Controller.h"
#include "leap/HandTriggerTracker.h"

class ClipController : public Controller
{
public:
	ClipController();
	void gainFocus() override;
	void loseFocus() override;
	bool leapInput(const Leap::Controller& controller, const Leap::Frame& frame) override;
	std::unique_ptr<Menu> contextMenu() override;

private:
	HandTriggerTracker hand_tracker_;
	unsigned cur_plane_;

	void updateVector(const Leap::Controller& controller);
};

#endif // __medleap_ClipController__
