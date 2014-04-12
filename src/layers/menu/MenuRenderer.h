#ifndef __medleap__MenuRenderer__
#define __medleap__MenuRenderer__

#include "layers/Renderer.h"
#include "MenuManager.h"
#include "gl/Buffer.h"
#include "gl/Program.h"
#include "math/Matrix4.h"
#include <functional>

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
    
    Mat4 modelViewProjection;
    gl::Buffer* menuVBO;
    gl::Buffer* menuIBO;
    gl::Program* menuShader;
    std::function<void(void)> setShaderState;
    GLsizei indexCount;
    GLenum indexType;
	GLsizei indicesPerMenuItem;
        
    void createRingGeometry();
    void createListGeometry();

	void drawMenu(Menu& menu);
};

#endif /* defined(__medleap__MenuRenderer__) */
