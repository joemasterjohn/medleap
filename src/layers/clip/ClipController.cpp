#include "ClipController.h"
#include "main/MainController.h"
#include "gl/math/Math.h"

using namespace gl;
using namespace std;
using namespace Leap;

static const int max_planes = 4;

ClipController::ClipController() : cur_plane_(0)
{
	hand_tracker_.trackFunction(bind(&ClipController::updateVector, this, placeholders::_1));
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
	auto& lsc = MainController::getInstance().leapStateController();
	std::set<LeapStateController::State> states;
	states.insert(LeapStateController::State::h1f1_point);
	lsc.availableStates(states);
}

void ClipController::loseFocus()
{
	hand_tracker_.tracking(false);
}

bool ClipController::leapInput(const Leap::Controller& controller, const Leap::Frame& frame)
{
	hand_tracker_.update(controller);

	return false;
}

void ClipController::updateVector(const Leap::Controller& controller)
{
	VolumeController& vc = MainController::getInstance().volumeController();

	if (vc.clipPlanes().empty()) {
		return;
	}

	Mat4 view_inverse = vc.getCamera().getView().rotScale().transpose();

	Vector t = hand_tracker_.deltaTipPos().normalized();
	Vec3 dir = view_inverse * Vec4{ t.x, t.y, t.z, 0.0f };
	vc.clipPlanes()[cur_plane_].normal(dir);


	vc.markDirty();
}