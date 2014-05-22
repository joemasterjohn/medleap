#include "LoadController.h"
#include "main/MainController.h"
#include "gl/math/Math.h"
#include "main/MainConfig.h"
#include "util/Util.h"

using namespace gl;
using namespace std;
using namespace Leap;

LoadController::LoadController() :
	changed_dir_(true),
	scroll_speed_(15.0f),
	highlighted_(nullptr),
	transition_(chrono::milliseconds(200)),
	cd_transition_(chrono::milliseconds(200)),
	timeout_(5000),
	timer_(0)
{
	MainConfig cfg;
	menu.directory(cfg.getValue<string>(MainConfig::WORKING_DIR));

	transition_.state(Transition::State::empty);
	cd_transition_.state(Transition::State::full);
	poses_.point().enabled(true);
	menu.onLoad(bind(&LoadController::source, this, std::placeholders::_1));
}

void LoadController::gainFocus()
{
	LeapStateController& lsc = MainController::getInstance().leapStateController();
	lsc.clear();
	lsc.add(LeapStateController::icon_h1f1_point, "Back");
	lsc.add(LeapStateController::icon_h1f1_point, "Choose");
	lsc.add(LeapStateController::icon_h1f1_point, "Scroll");
	transition_.state(Transition::State::increase);
	timer_ = chrono::milliseconds(0);
}

void LoadController::loseFocus()
{
	transition_.state(Transition::State::decrease);
}

void LoadController::resize()
{
	list_renderer_.update(menu, viewport_);

	boundary_width_ = viewport_.width * 0.05f;
	boundary_left_ = boundary_width_;
	boundary_right_ = viewport_.width - boundary_width_;

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
	if (!transition_.full()) {
		return false;
	}

	poses_.update(frame);

	if (poses_.point().tracking()) {

		LeapStateController& lsc = MainController::getInstance().leapStateController();
		lsc.active(LeapStateController::icon_none);

		Vector v = frame.interactionBox().normalizePoint(poses_.point().pointer().stabilizedTipPosition());
		updateCursor(v.x * viewport_.width, v.y * viewport_.height);

		if  (cd_transition_.state() == Transition::State::full) {
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
	}

	return false;
}

void LoadController::scroll(float amount)
{
	LeapStateController& lsc = MainController::getInstance().leapStateController();
	lsc.active(LeapStateController::icon_h1f1_point);
	y_offset_ = clamp(y_offset_ + scroll_speed_ * amount, 0.0f, max(0.0f ,list_renderer_.contentHeight() - viewport_.height));
	list_renderer_.model(translation(0, y_offset_, 0));
}

void LoadController::upDirectory()
{
	cd_direction_ = 1.0f;
	cd_transition_.state(Transition::State::decrease);
	menu.upDirectory();
	changed_dir_ = true;
}

void LoadController::intoDirectory()
{
	if (highlighted_) {
		cd_direction_ = -1.0f;
		cd_transition_.state(Transition::State::decrease);
		highlighted_->trigger();
		changed_dir_ = true;
	}
}

void LoadController::update(chrono::milliseconds elapsed)
{
	updateTransition(elapsed);

	if (loader.getState() == VolumeLoader::LOADING) {
		state_renderer_.update(loader, elapsed, viewport_);
	}

	if (loader.getState() == VolumeLoader::FINISHED) {
		MainController::getInstance().setVolume(loader.getVolume());
		MainController::getInstance().volumeController().markDirty();
	}

	if (transition_.full()) {
		if (poses_.point().tracking()) {
			timer_ = chrono::milliseconds(0);
		} else {
			timer_ += elapsed;
			if (timer_ >= timeout_) {
				MainController::getInstance().popFocus();
			}
		}
	}
}

void LoadController::updateTransition(chrono::milliseconds elapsed)
{
	transition_.update(elapsed);
	list_renderer_.alpha(transition_.progress());

	cd_transition_.update(elapsed);
	switch (cd_transition_.state()) {
	case Transition::State::decrease:
		list_renderer_.model(translation(cd_direction_ * (1.0f - cd_transition_.progress()) * viewport_.width * 1.15f, y_offset_, 0));
		break;
	case Transition::State::empty:
		cd_transition_.state(Transition::State::increase);
		list_renderer_.update(menu, viewport_);
		y_offset_ = 0.0f;
		break;
	case Transition::State::increase:
		list_renderer_.model(translation(-cd_direction_ * (1.0f - cd_transition_.progress()) * viewport_.width * 1.15f, y_offset_, 0));
		break;
	case Transition::State::full:
		list_renderer_.model(translation(0, y_offset_, 0));
		break;
	}
}

void LoadController::draw()
{
	if (loader.getState() == VolumeLoader::LOADING) {
		state_renderer_.draw();
	} else if (!transition_.empty()) {
		list_renderer_.draw();

		// leap cursor
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
	MainController::getInstance().popFocus();
}