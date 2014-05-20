#include "LoadController.h"
#include "main/MainController.h"
#include "gl/math/Math.h"
#include "main/MainConfig.h"
#include "util/Util.h"

using namespace gl;
using namespace std;
using namespace Leap;

LoadController::LoadController() :
	visible_(false), 
	changed_dir_(false),
	boundary_width_(0.15f),
	boundary_height_(0.15f),
	scroll_speed_(15.0f),
	highlighted_(nullptr)
{
	MainConfig cfg;
	menu.directory(cfg.getValue<string>(MainConfig::WORKING_DIR));

	poses_.point().enabled(true);
}

void LoadController::gainFocus()
{
	auto& lsc = MainController::getInstance().leapStateController();
	lsc.clear();
	lsc.add(LeapStateController::icon_h1f1_circle, "Menu");
	visible_ = true;
}

void LoadController::loseFocus()
{
	visible_ = false;
}

void LoadController::resize()
{
	list_renderer_.update(menu, viewport_);

	boundary_width_ = viewport_.width * 0.15f;
	boundary_left_ = viewport_.left() + boundary_width_;
	boundary_right_ = viewport_.right() - boundary_width_;

	boundary_height_ = viewport_.height * 0.15f;
	boundary_bottom_ = viewport_.bottom() + boundary_height_;
	boundary_top_ = viewport_.top() - boundary_height_;

	scroll(0.0f);
}

void LoadController::updateCursor(float x, float y)
{
	cursor_.x = x;
	cursor_.y = y;
	if (menu.items().empty()) {
		highlighted_ = nullptr;
	} else {
		float y_world = viewport_.height - y + y_offset_;
		float item_step = list_renderer_.itemHeight() + list_renderer_.verticalPad();
		int i = static_cast<int>(y_world / item_step);
		i = clamp(i, 0, static_cast<int>(menu.items().size()) - 1);
		highlighted_ = &menu.getItems()[i];
		list_renderer_.highlight(i);
	}
}

bool LoadController::leapInput(const Leap::Controller& controller, const Leap::Frame& frame)
{
	poses_.update(frame);

	if (poses_.point().tracking()) {
		Vector v = frame.interactionBox().normalizePoint(poses_.point().pointer().tipPosition());
		updateCursor(v.x * viewport_.width, v.y * viewport_.height);

		if (cursor_.y < boundary_bottom_) {
			scroll((boundary_bottom_ - cursor_.y) / boundary_height_);
		} else if (cursor_.y > boundary_top_) {
			scroll(-(cursor_.y - boundary_top_) / boundary_height_);
		}

		if (cursor_.x < boundary_left_ && !changed_dir_) {
			upDirectory();
		} else if (cursor_.x > boundary_right_ && !changed_dir_) {
			intoDirectory();
		} else if (cursor_.x > boundary_left_ && cursor_.x < boundary_right_ && changed_dir_) {
			changed_dir_ = false;
		}
	}

	return false;
}

void LoadController::scroll(float amount)
{
	y_offset_ = clamp(y_offset_ + scroll_speed_ * amount, 0.0f, max(0.0f ,list_renderer_.contentHeight() - viewport_.height));
	if (y_offset_ < 0) {
		cout << list_renderer_.contentHeight() << endl;
	}
	list_renderer_.model(translation(0, y_offset_, 0));
}

void LoadController::upDirectory()
{
	menu.upDirectory();
	changed_dir_ = true;
	list_renderer_.update(menu, viewport_);
	y_offset_ = 0.0f;
	list_renderer_.model(translation(0, y_offset_, 0));
}

void LoadController::intoDirectory()
{
	if (highlighted_) {
		highlighted_->trigger();
		changed_dir_ = true;
		list_renderer_.update(menu, viewport_);
		y_offset_ = 0.0f;
		list_renderer_.model(translation(0, y_offset_, 0));
	}
}

void LoadController::update(chrono::milliseconds elapsed)
{
	if (loader.getState() == VolumeLoader::FINISHED) {
		MainController::getInstance().setVolume(loader.getVolume());
		MainController::getInstance().volumeController().markDirty();
	}
}

void LoadController::draw()
{
	static float f = 0.0f;
	if (loader.getState() == VolumeLoader::LOADING) {
		GLclampf c = static_cast<GLclampf>((std::sin(f += 0.01) * 0.5 + 0.5) * 0.5 + 0.5);
		viewport_.apply();

		//TextRenderer& text = MainController::getInstance().getText();
		//text.setColor(1, c, c);
		//text.begin(viewport_.width, viewport_.height);
		//text.add(string("Loading"), viewport_.width / 2, viewport_.height / 2, TextRenderer::CENTER, TextRenderer::CENTER);
		//text.add(loader.getStateMessage(), viewport_.width / 2, viewport_.height / 2 - 36, TextRenderer::CENTER, TextRenderer::CENTER);
		//text.end();
	} else if (visible_) {
		list_renderer_.draw();

		static Draw d;
		d.setModelViewProj(viewport_.orthoProjection());
		d.begin(GL_TRIANGLE_FAN);
		d.color(0, 0, 0);
		d.begin(GL_TRIANGLE_FAN);
		d.circle(cursor_.x, cursor_.y, 15.0f, 32);
		d.end();
		d.draw();
		d.begin(GL_TRIANGLE_FAN);
		d.color(1.0f, 1.0f, 1.0f);
		d.circle(cursor_.x, cursor_.y, 13.0f, 32);
		d.end();
		d.draw();
	}
}

void LoadController::source(const VolumeLoader::Source& source)
{
	loader.setSource(source);
}