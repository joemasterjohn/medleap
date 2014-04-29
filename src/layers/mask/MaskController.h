#ifndef __medleap_MaskController__
#define __medleap_MaskController__

#include "layers/Controller.h"

class MaskController : public Controller
{
public:
	MaskController();

	bool leapInput(const Leap::Controller& controller, const Leap::Frame& frame) override;

};

#endif // __medleap_MaskController__
