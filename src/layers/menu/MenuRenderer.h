#ifndef __medleap__MenuRenderer__
#define __medleap__MenuRenderer__

#include "layers/Renderer.h"

class MenuRenderer : public Renderer
{
public:
	MenuRenderer();
	~MenuRenderer();

	void init();
	void draw();
	void resize(int width, int height);

private:

};

#endif /* defined(__medleap__MenuRenderer__) */
