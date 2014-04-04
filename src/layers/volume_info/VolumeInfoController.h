#ifndef __medleap__VolumeInfoController__
#define __medleap__VolumeInfoController__

#include "layers/Controller.h"
#include "VolumeInfoRenderer.h"

class VolumeInfoController : public Controller
{
public:
    VolumeInfoController();
    ~VolumeInfoController();
    VolumeInfoRenderer* getRenderer();
    void setVolume(VolumeData* volume);
    
private:
    VolumeInfoRenderer renderer;
};

#endif /* defined(__medleap__VolumeInfoController__) */
