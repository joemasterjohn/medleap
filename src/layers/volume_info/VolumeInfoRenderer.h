#ifndef __medleap__VolumeInfoRenderer__
#define __medleap__VolumeInfoRenderer__

#include "gl/glew.h"
#include "layers/Renderer.h"
#include "data/VolumeData.h"
#include "layers/volume/VolumeRenderer.h"
#include "layers/slice/SliceRenderer.h"
#include <string>

/** Draws informational text for the volume data */
class VolumeInfoRenderer : public Renderer
{
public:
    void init();
    void draw();
    void setVolume(VolumeData* volume);
    void setVolumeRenderer(VolumeRenderer* renderer);
    void setSliceRenderer(SliceRenderer* renderer);
    void resize(int width, int height);
    
private:
    int windowWidth;
    int windowHeight;
    VolumeData* volume;
    VolumeRenderer* volumeRenderer;
    SliceRenderer* sliceRenderer;
    
    void drawText(std::string& str, int row);
};

#endif /* defined(__medleap__VolumeInfoRenderer__) */
