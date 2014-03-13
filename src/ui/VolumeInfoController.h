#ifndef __medleap__VolumeInfoController__
#define __medleap__VolumeInfoController__

#include "ui/Controller.h"
#include "render/VolumeInfoRenderer.h"

class VolumeInfoController : public Controller
{
public:
    VolumeInfoController();
    ~VolumeInfoController();
    VolumeInfoRenderer* getRenderLayer();
    void setVolume(VolumeData* volume);
    
private:
    VolumeInfoRenderer renderer;
};

#endif /* defined(__medleap__VolumeInfoController__) */
