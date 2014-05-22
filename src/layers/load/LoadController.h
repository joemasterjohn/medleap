#ifndef __medleap_LoadController__
#define __medleap_LoadController__

#include "layers/Controller.h"
#include "data/VolumeLoader.h"
#include "DirectoryMenu.h"
#include "ListRenderer.h"
#include "LoadStateRenderer.h"
#include "leap/PoseTracker.h"
#include "util/Transition.h"

class LoadController : public Controller
{
public:
	LoadController();
	void gainFocus() override;
	void loseFocus() override;
	void update(std::chrono::milliseconds elapsed) override;
	void draw() override;
	void resize() override;
	bool modal() const override { return true; }
	bool leapInput(const Leap::Controller& controller, const Leap::Frame& frame) override;
	void source(const VolumeLoader::Source& source);

private:
	VolumeLoader loader;
	DirectoryMenu menu;
	ListRenderer list_renderer_;
	LoadStateRenderer state_renderer_;
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
	Transition transition_;
	Transition cd_transition_;
	float cd_direction_;
	std::chrono::milliseconds timeout_;
	std::chrono::milliseconds timer_;

	void updateTransition(std::chrono::milliseconds elapsed);
	void updateCursor(float x, float y);
	void scroll(float amount);
	void upDirectory();
	void intoDirectory();
};

#endif // __medleap_LoadController__
