#ifndef __medleap_FocusController__
#define __medleap_FocusController__

#include "layers/Controller.h"

class FocusController : public Controller
{
public:
	FocusController();

	bool leapInput(const Leap::Controller& controller, const Leap::Frame& frame) override;

};

#endif // __medleap_FocusController__
