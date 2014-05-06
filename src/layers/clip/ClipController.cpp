#include "ClipController.h"
#include "main/MainController.h"
#include "gl/math/Math.h"

using namespace gl;
using namespace std;
using namespace Leap;

static const int max_planes = 4;

ClipController::ClipController() : cur_plane_(0)
{
	grab_tracker_.trackFunction(bind(&ClipController::updateVector, this, placeholders::_1));
}

std::unique_ptr<Menu> ClipController::contextMenu()
{
	Menu* menu = new Menu("Clipping");
	MenuItem& mi_new = menu->createItem("New Plane");
	mi_new.setAction([&]{
		auto& vc = MainController::getInstance().volumeController();
		if (vc.clipPlanes().size() < max_planes) {
			vc.clipPlanes().push_back(Plane(Vec3::yAxis(), 0.0f));
			cur_plane_ = vc.clipPlanes().size() - 1;
		}
		vc.markDirty();
		MainController::getInstance().menuController().hideMenu();
	});

	MenuItem& mi_delete = menu->createItem("Delete Plane");
	mi_delete.setAction([&]{
		auto& vc = MainController::getInstance().volumeController();
		if (!vc.clipPlanes().empty()) {
			vc.clipPlanes().erase(vc.clipPlanes().begin() + cur_plane_);
			cur_plane_ = 0;
		}
		vc.markDirty();
		MainController::getInstance().menuController().hideMenu();
	});

	MenuItem& mi_clear = menu->createItem("Clear");
	mi_clear.setAction([&]{
		auto& vc = MainController::getInstance().volumeController();
		vc.clipPlanes().clear();
		vc.markDirty();
		cur_plane_ = 0;
	});

	return std::unique_ptr<Menu>(menu);
}

void ClipController::gainFocus()
{
	MainController::getInstance().setMode(MainController::MODE_3D);
	auto& lsc = MainController::getInstance().leapStateController();
	lsc.clear();
	lsc.add(LeapStateController::icon_h1f1_circle, "Main Menu");
	lsc.add(LeapStateController::icon_h1f2_circle, "Options");
	lsc.add(LeapStateController::icon_h1f2_trigger, "Rotate Plane");

	VolumeController& vc = MainController::getInstance().volumeController();
	vc.draw_bounds = true;
	vc.draw_planes = true;
	vc.markDirty();
}

void ClipController::loseFocus()
{
	VolumeController& vc = MainController::getInstance().volumeController();
	vc.draw_bounds = false;
	vc.draw_planes = false;
	vc.markDirty();
	grab_tracker_.tracking(false);
}

bool ClipController::leapInput(const Leap::Controller& controller, const Leap::Frame& frame)
{
	if (!cam_control_.tracking()) {
		cut_tracker_.update(controller);
	}

	if (!cut_tracker_.tracking()) {
		cam_control_.update(controller);
	}

	if (cut_tracker_.state() != CutTracker::State::searching) {
		Vector p = cut_tracker_.handCurrent().stabilizedPalmPosition();
		p = frame.interactionBox().normalizePoint(p);
		leap_current_ = Vec2(p.x * viewport_.width, p.y * viewport_.width) + Vec2(viewport_.x, viewport_.y);

		if (cut_tracker_.state() != CutTracker::State::tracking) {
			leap_start_ = leap_current_;
		} else {
			Vector end = cut_tracker_.handCurrent().stabilizedPalmPosition();
			end = frame.interactionBox().normalizePoint(end);
			leap_end_ = Vec2(end.x * viewport_.width, end.y * viewport_.width) + Vec2(viewport_.x, viewport_.y);

			VolumeController& vc = MainController::getInstance().volumeController();
			if (!vc.clipPlanes().empty()) {
				Mat4 inv = (vc.getCamera().getProjection() * vc.getCamera().getView()).inverse();

				auto unproject = [&](const Vec2& p, float z)->Vec4 {
					Vec2 ndc = (viewport_.normalize(p) - 0.5f) * 2.0f;
					Vec4 result = inv * Vec4(ndc.x, ndc.y, z, 1.0f);
					return result / result.w;
				};

				Vec3 sn = unproject(leap_start_, -1.0f);
				Vec3 sf = unproject(leap_start_, +1.0f);
				Vec3 en = unproject(leap_end_, -1.0f);
				Vec3 n = (en - sn).cross(sf - sn).normalize();

				vc.clipPlanes()[cur_plane_] = Plane(n, sn);
				vc.markDirty();
			}
		}
	}

	//grab_tracker_.update(controller);

	return false;
}

void ClipController::updateVector(const Leap::Controller& controller)
{
	VolumeController& vc = MainController::getInstance().volumeController();

	if (vc.clipPlanes().empty()) {
		return;
	}

	Mat4 view_inverse = vc.getCamera().getView().rotScale().transpose();

	//Vector t = hand_tracker_.deltaTipPos().normalized();
	//Vec3 dir = view_inverse * Vec4{ t.x, t.y, t.z, 0.0f };
	//vc.clipPlanes()[cur_plane_].normal(dir);

	grab_tracker_.palmPosDelta().normalized();


	vc.markDirty();
}

void ClipController::draw()
{
	Draw& d = MainController::getInstance().draw();

	if (cut_tracker_.tracking()) {
		d.setModelViewProj(viewport_.orthoProjection());
		d.begin(GL_LINES);
		d.color(1, 0, 0);
		Vec2 c = (leap_end_ - leap_start_) / 2 + leap_start_;
		Vec2 n = (leap_end_ - leap_start_).normalize().rotate90() * 50;
		d.vertex(c.x , c.y , 0);
		d.vertex(c.x + n.x, c.y + n.y, 0);
		d.end();
		d.draw();
	}

	if (cut_tracker_.state() != CutTracker::State::searching) {
		d.setModelViewProj(viewport_.orthoProjection());
		d.color(0, 0, 0);
		d.begin(GL_TRIANGLE_FAN);
		d.circle(leap_current_.x, leap_current_.y, 15.0f, 32);
		d.end();
		d.draw();

		if (cut_tracker_.tracking()) {
			d.color(1, .5f, .5f);
		} else {
			d.color(1, 1, .5f);
		}
		d.begin(GL_TRIANGLE_FAN);
		d.circle(leap_current_.x, leap_current_.y, 13.0f, 32);
		d.end();
		d.draw();
	}
}