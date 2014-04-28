#ifndef __medleap__MenuRenderer__
#define __medleap__MenuRenderer__

#include "layers/Renderer.h"
#include "MenuManager.h"
#include "gl/Buffer.h"
#include "gl/Program.h"
#include "gl/math/Math.h"
#include <functional>

class MenuRenderer : public Renderer
{
public:
	MenuRenderer();
	~MenuRenderer();

	void draw() override;
	void resize(int width, int height) override;
	void menu(Menu* menu);
	void highlight(int menuIndex);

	float progress_;
	gl::Vec2 leap;
	float visibility_;

private:
	Menu* menu_;
	int highlighted;
    
	gl::Mat4 modelViewProjection;
    gl::Buffer menuVBO;
    gl::Buffer menuIBO;
    gl::Program menuShader;
    std::function<void(void)> setShaderState;
    GLsizei indexCount;
    GLenum indexType;
	GLsizei indicesPerMenuItem;
        
    void createRingGeometry();
    void createListGeometry();

	void drawMenu(Menu& menu, gl::Vec3 tc1, gl::Vec3 tc2);
};

#endif /* defined(__medleap__MenuRenderer__) */
