#ifndef __medleap__MenuRenderer__
#define __medleap__MenuRenderer__

#include "layers/Renderer.h"
#include "MenuManager.h"

class MenuRenderer : public Renderer
{
public:
	MenuRenderer(MenuManager* menuManager);
	~MenuRenderer();

	void init();
	void draw();
	void resize(int width, int height);
	void highlight(int menuIndex);

private:
	MenuManager* menuManager;
	int highlighted;

	void drawMenu(Menu& menu);
};

#endif /* defined(__medleap__MenuRenderer__) */
