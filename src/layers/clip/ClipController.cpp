#include "ClipController.h"
#include "main/MainController.h"
#include "gl/math/Math.h"
#include "util/Util.h"

using namespace gl;
using namespace std;
using namespace Leap;

static const int max_planes = 4;

ClipController::ClipController() : selected_plane_(0)
{
}

std::unique_ptr<Menu> ClipController::contextMenu()
{
	Menu* menu = new Menu("Clipping");
	MenuItem& mi_new = menu->createItem("New Plane");
	mi_new.setAction([&]{
		addPlane(Plane(Vec3::yAxis(), 2.0f));
		MainController::getInstance().menuController().hideMenu();
	});

	MenuItem& mi_delete = menu->createItem("Delete Plane");
	mi_delete.setAction([&]{
		deleteSelectedPlane();
		MainController::getInstance().menuController().hideMenu();
	});

	MenuItem& mi_clear = menu->createItem("Clear");
	mi_clear.setAction([&]{
		clearPlanes();
		MainController::getInstance().menuController().hideMenu();
	});

	return std::unique_ptr<Menu>(menu);
}

void ClipController::gainFocus()
{
	MainController::getInstance().showTransfer1D(false);
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
	v_pose_.tracking(false);
}

Plane& ClipController::selectedPlane()
{
	vector<Plane>& planes = MainController::getInstance().volumeController().clipPlanes();
	if (planes.empty()) {
		addPlane(Plane(Vec3::yAxis(), 2.0f));
	}
	return planes[selected_plane_];
}

void ClipController::addPlane(const Plane& plane)
{
	VolumeController& vc = MainController::getInstance().volumeController();
	vector<Plane>& planes = MainController::getInstance().volumeController().clipPlanes();
	if (planes.size() < max_planes) {
		planes.push_back(plane);
		selected_plane_ = planes.size() - 1;
		vc.markDirty();
	}
}

void ClipController::deleteSelectedPlane()
{
	VolumeController& vc = MainController::getInstance().volumeController();
	if (!vc.clipPlanes().empty()) {
		vc.clipPlanes().erase(vc.clipPlanes().begin() + selected_plane_);
		selected_plane_ = 0;
		vc.markDirty();
	}
}

void ClipController::clearPlanes()
{
	VolumeController& vc = MainController::getInstance().volumeController();
	if (!vc.clipPlanes().empty()) {
		vc.clipPlanes().clear();
		vc.markDirty();
		selected_plane_ = 0;
	}
}

void ClipController::clip1H(const Leap::Frame& frame)
{
	leap_current_ = denormalize(viewport_, frame, v_pose_.hand().stabilizedPalmPosition());

	if (!v_pose_.isClosed()) {
		leap_start_ = leap_current_;
	} else {
		leap_end_ = denormalize(viewport_, frame, v_pose_.hand().stabilizedPalmPosition());

		VolumeController& vc = MainController::getInstance().volumeController();

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

		selectedPlane().normal(n);
		selectedPlane().point(sn);
		vc.markDirty();
	}
}

void ClipController::clip2H(const Leap::Frame& frame)
{
	Hand hand = frame.hands().frontmost();
	Hand other_hand = frame.hands()[0].id() == hand.id() ? frame.hands()[1] : frame.hands()[0];

	if (other_hand.grabStrength() > 0.8f) {
		VolumeController& vc = MainController::getInstance().volumeController();

		Mat4 inv = vc.getCamera().getView().inverse();
		Vec3 n = inv * hand.palmNormal().toVector4<Vec4>();
		Vec3 p = frame.interactionBox().normalizePoint(hand.palmPosition()).toVector4<Vec4>();
		p = (p - 0.5f) * 2.0f;
		p = inv * Vec4(p.x, p.y, p.z, 0.0f);

		selectedPlane().normal(n);
		selectedPlane().point(p);
		vc.markDirty();
	}
}

bool ClipController::leapInput(const Leap::Controller& controller, const Leap::Frame& frame)
{
	if (frame.hands().count() == 2) {
		clip2H(frame);
		return false;
	}

	v_pose_.update(frame);
	if (v_pose_.tracking()) {
		clip1H(frame);
		return false;
	}

	cam_control_.update(controller, frame);
	return false;
}

void ClipController::draw()
{
	Draw& d = MainController::getInstance().draw();

	if (v_pose_.tracking()) {
		d.setModelViewProj(viewport_.orthoProjection());
		d.color(0, 0, 0);
		d.begin(GL_TRIANGLE_FAN);
		d.circle(leap_current_.x, leap_current_.y, 15.0f, 32);
		d.end();
		d.draw();

		d.begin(GL_TRIANGLE_FAN);
		d.circle(leap_start_.x, leap_start_.y, 15.0f, 32);
		d.end();
		d.draw();

		if (v_pose_.isClosed()) {
			d.color(1, .5f, .5f);
		} else {
			d.color(1, 1, .5f);
		}
		d.begin(GL_TRIANGLE_FAN);
		d.circle(leap_current_.x, leap_current_.y, 13.0f, 32);
		d.end();
		d.draw();

		d.begin(GL_TRIANGLE_FAN);
		d.circle(leap_start_.x, leap_start_.y, 13.0f, 32);
		d.end();
		d.draw();

		if (v_pose_.isClosed()) {
			d.setModelViewProj(viewport_.orthoProjection());
			d.begin(GL_LINES);
			d.color(1, 0, 0);
			Vec2 c = (leap_end_ - leap_start_) / 2 + leap_start_;
			Vec2 n = (leap_end_ - leap_start_).normalize().rotate90() * 50;
			d.vertex(c.x, c.y, 0);
			d.vertex(c.x + n.x, c.y + n.y, 0);
			d.end();
			d.draw();
		}
	}
}