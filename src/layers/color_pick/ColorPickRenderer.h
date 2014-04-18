#ifndef __medleap__ColorPickRenderer__
#define __medleap__ColorPickRenderer__

#include "layers/Renderer.h"

class ColorPickRenderer : public Renderer
{
public:
	void draw() override;
	void resize(int width, int height) override;
};

#endif // __medleap__ColorPickRenderer__
