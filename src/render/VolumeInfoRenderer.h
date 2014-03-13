#ifndef __medleap__VolumeInfoRenderer__
#define __medleap__VolumeInfoRenderer__

#include <GL/glew.h>
#include "render/RenderLayer.h"
#include "util/TextRenderer.h"
#include "volume/VolumeData.h"

/** Draws informational text for the volume data */
class VolumeInfoRenderer : public RenderLayer
{
public:
    void init();
    void draw();
    void setVolume(VolumeData* volume);
    void resize(int width, int height);
    
private:
    TextRenderer text;
    int windowWidth;
    int windowHeight;
    VolumeData* volume;
};

#endif /* defined(__medleap__VolumeInfoRenderer__) */
