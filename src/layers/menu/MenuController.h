#ifndef __medleap__MenuController__
#define __medleap__MenuController__

#include "layers/Controller.h"
#include "gl/Buffer.h"
#include "gl/Program.h"
#include "gl/math/Math.h"
#include "leap/OneFingerTracker.h"
#include "util/Transition.h"
#include <memory>
#include <functional>


class MenuController : public Controller
{
public:
	MenuController();

	bool keyboardInput(GLFWwindow* window, int key, int action, int mods) override;
	bool mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y) override;
	bool mouseMotion(GLFWwindow* window, double x, double y) override;
	bool leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame) override;
	void update(std::chrono::milliseconds elapsed) override;
	
	void hideMenu();
	void showMainMenu();
	void showContextMenu();

	void draw() override;

private:
	Transition transition_;
    std::string workingDir;
	std::unique_ptr<Menu> menu_;
	int selected_;
	OneFingerTracker finger_tracker_;

	// renderer (clean up)
	float progress_; // leap progress, not transition
	gl::Vec2 leap; // leap cursor
	gl::Mat4 modelViewProjection;
	gl::Buffer menuVBO;
	gl::Buffer menuIBO;
	gl::Program menuShader;
	std::function<void(void)> setShaderState;
	GLsizei indexCount;
	GLenum indexType;
	GLsizei indicesPerMenuItem;


	void resize() override;
	int calcHighlightedMenu(double x, double y);
	int calcHighlightedMenu(double radians);    

	void createRingGeometry();
	void createListGeometry();
	void drawMenu(Menu& menu, gl::Vec3 tc1, gl::Vec3 tc2);
};

#endif /* defined(__medleap__MenuController__) */
