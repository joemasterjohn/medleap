#ifndef medleap_ColorPickController_h
#define medleap_ColorPickController_h

#include "layers/Controller.h"
#include "ColorPickRenderer.h"

class ColorPickController : public Controller
{
public:
	ColorPickRenderer* getRenderer() override { return &renderer; }

private:
	ColorPickRenderer renderer;
};

#endif // medleap_ColorPickController_h
