#ifndef __medleap__VolumeInfoController__
#define __medleap__VolumeInfoController__

#include "gl/glew.h"
#include "layers/Controller.h"
#include "data/VolumeData.h"
#include "layers/volume/VolumeController.h"
#include "layers/slice/SliceController.h"
#include <string>

class VolumeInfoController : public Controller
{
public:
    VolumeInfoController();

	void setVolume(VolumeData* volume);
	void setVolumeRenderer(VolumeController* renderer);
	void setSliceRenderer(SliceController* renderer);

	void draw() override;
    
private:
	VolumeData* volume;
	VolumeController* volumeRenderer;
	SliceController* sliceRenderer;

	void drawText(const std::string& str, int row);
};

#endif /* defined(__medleap__VolumeInfoController__) */
