#ifndef __medleap_OrientationController__
#define __medleap_OrientationController__

#include "layers/Controller.h"
#include "OrientationRenderer.h"
#include "util/Camera.h"
#include "data/VolumeData.h"

class OrientationController : public Controller
{
public:
	OrientationController() : camera(NULL) {}
	OrientationRenderer* getRenderer() override { return &renderer; }
	void setCamera(Camera* camera) { this->camera = camera; renderer.setCamera(camera); }
	void setVolume(VolumeData* volume) { renderer.setVolume(volume); }
private:
	OrientationRenderer renderer;
	Camera* camera;
};

#endif // __medleap_OrientationController__
