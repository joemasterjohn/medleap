#ifndef __medleap_LoadController__
#define __medleap_LoadController__

#include "layers/Controller.h"
#include "data/VolumeLoader.h"
#include "DirectoryMenu.h"
#include "ListRenderer.h"
#include "leap/PoseTracker.h"

class LoadController : public Controller
{
public:
	LoadController();
	void gainFocus() override;
	void loseFocus() override;
	void update(std::chrono::milliseconds elapsed) override;
	void draw() override;
	void resize() override;
	bool leapInput(const Leap::Controller& controller, const Leap::Frame& frame) override;
	void source(const VolumeLoader::Source& source);

private:
	VolumeLoader loader;
	DirectoryMenu menu;
	ListRenderer list_renderer_;
	bool visible_;
	float y_offset_;
	PoseTracker poses_;
	gl::Vec2 cursor_;
	bool changed_dir_;
	float scroll_speed_;
	float boundary_width_;
	float boundary_left_;
	float boundary_right_;
	float boundary_height_;
	float boundary_bottom_;
	float boundary_top_;
	MenuItem* highlighted_;

	void updateCursor(float x, float y);
	void scroll(float amount);
	void upDirectory();
	void intoDirectory();
};

#endif // __medleap_LoadController__
