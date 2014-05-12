#include "MenuController.h"
#include <iostream>
#include "util/Util.h"
#include "gl/math/Math.h"
#include "main/MainController.h"

#include "DirectoryMenu.h"
#include "MainMenu.h"

using namespace std;
using namespace gl;


MenuController::MenuController() :
	transition_(std::chrono::milliseconds(250)),
	leap_state_(LeapState::closed)
{
	point_pose_.engageDelay(std::chrono::milliseconds(0));
	point_pose_.disengageDelay(std::chrono::milliseconds(0));
	point_pose_.engageSpeedThreshold(1200);

	menuVBO.generateVBO(GL_STATIC_DRAW);
	menuIBO.generateIBO(GL_STATIC_DRAW);
	menuShader = Program::create("shaders/menu.vert", "shaders/menu.frag");
}

void MenuController::loseFocus()
{
	point_pose_.tracking(false);
}

void MenuController::hideMenu()
{
	leap_state_ = LeapState::closed;
	point_pose_.tracking(false);
	transition_.state(Transition::State::decrease);
	MainController::getInstance().leapStateController().active(LeapStateController::icon_none);
}

void MenuController::showMainMenu()
{
	leap_state_ = LeapState::open;
	Menu* m = new MainMenu;
	menu_ = std::unique_ptr<Menu>(m);
	transition_.state(Transition::State::increase);
	createRingGeometry();
}

void MenuController::showContextMenu()
{
	leap_state_ = LeapState::open;
	Controller* focusLayer = MainController::getInstance().focusLayer();
	if (focusLayer) {
		unique_ptr<Menu> m = focusLayer->contextMenu();
		if (m) {
			menu_ = move(m);
			transition_.state(Transition::State::increase);
			createRingGeometry();
		}
	}
}

bool MenuController::keyboardInput(GLFWwindow* window, int key, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		if (transition_.full()) {
			hideMenu();
		} else if (transition_.empty()) {
			showMainMenu();
		}
	}
	else if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
		if (transition_.full()) {
			hideMenu();
		} else if (transition_.empty()) {
			showContextMenu();
		}
	}

	return transition_.empty();
}

bool MenuController::mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y)
{
	if (transition_.empty())
		return true;

	if (transition_.full() && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		int selected = calcHighlightedMenu(x, y);
		(*menu_)[selected].trigger();
	}

	return false;
}

bool MenuController::mouseMotion(GLFWwindow* window, double x, double y)
{
	if (transition_.empty())
		return true;

	int item = calcHighlightedMenu(x, y);

	return false;
}

int MenuController::calcHighlightedMenu(double x, double y)
{
    x = (x - viewport_.width / 2.0);
    y = (y - viewport_.height / 2.0);
	double radians = Vec2d(x, y).anglePositive();
	return calcHighlightedMenu(radians);
}

int MenuController::calcHighlightedMenu(double radians)
{
	if (!menu_)
		return -1;

	size_t numItems = menu_->getItems().size();
	double angleStep = gl::two_pi / numItems;
	return static_cast<int>(radians / angleStep) % numItems;
}

void MenuController::updateLeapPointer(const Leap::Controller& controller, const Leap::Frame& frame)
{
	point_pose_.update(controller);
	if (point_pose_.tracking()) {
		Leap::Finger f = point_pose_.index();
		Leap::Vector n = frame.interactionBox().normalizePoint(f.tipPosition());
		leap.x = n.x * viewport_.width + viewport_.x;
		leap.y = n.y * viewport_.height + viewport_.y;
	}
}

void MenuController::leapMenuClosed(const Leap::Controller& controller, const Leap::Frame& frame)
{
	Leap::GestureList gestures = frame.gestures();
	for (const Leap::Gesture& g : gestures) {
		if (g.type() == Leap::Gesture::TYPE_CIRCLE) {
			Leap::CircleGesture circle(g);
			bool xyPlane = abs(circle.normal().dot(Leap::Vector(0, 0, 1))) > 0.8f;
			if (circle.progress() > 1 && xyPlane && circle.radius() > 35.0f) {

				Leap::Hand hand = circle.hands().frontmost();
				Leap::FingerList fingers = hand.fingers();
				
				bool main = fingers.extended().count() == 1 && 
					fingers.extended()[0].type() == Leap::Finger::TYPE_INDEX;

				bool secondary = fingers[Leap::Finger::TYPE_INDEX].isExtended() &&
					fingers[Leap::Finger::TYPE_THUMB].isExtended() &&
					fingers[Leap::Finger::TYPE_MIDDLE].isExtended();

				if (main) {
					leap_state_ = LeapState::triggered_main;
				} else if (secondary && MainController::getInstance().focusLayer() && MainController::getInstance().focusLayer()->contextMenu()) {
					leap_state_ = LeapState::triggered_context;
				}
			}
		}
	}
}

void MenuController::leapMenuTriggered(const Leap::Controller& controller, const Leap::Frame& frame)
{
	updateLeapPointer(controller, frame);

	if (leap_state_ == LeapState::triggered_main) {
		MainController::getInstance().leapStateController().active(LeapStateController::icon_h1f1_circle);
	} else if (leap_state_ == LeapState::triggered_context) {
		leap_state_ = LeapState::triggered_context;
		MainController::getInstance().leapStateController().active(LeapStateController::icon_h1f2_circle);
	}

	if (point_pose_.tracking()) {
		float d = (leap - viewport_.center()).length();
		if (d < 35) {
			if (leap_state_ == LeapState::triggered_context) {
				showContextMenu();
			} else {
				showMainMenu();
			}
		}
	}
}

void MenuController::leapMenuOpen(const Leap::Controller& controller, const Leap::Frame& frame)
{
	updateLeapPointer(controller, frame);

	if (point_pose_.tracking()) {
		Vec2 v = (leap - viewport_.center());
		float radians = v.anglePositive();
		float radius = v.length();

		float innerRadius = min(viewport_.width, viewport_.height) * 0.5 * 0.55;
		float hitRadius = min(viewport_.width, viewport_.height) * 0.5 * 0.7;
		float outerRadius = min(viewport_.width, viewport_.height) * 0.5 * 0.85;
		selected_ = -1;
		if (radius > innerRadius) {
			int highlightedItem = calcHighlightedMenu(radians);
			selected_ = highlightedItem;
			progress_ = gl::clamp((radius - innerRadius) / (outerRadius - innerRadius), 0.0f, 1.0f);

			if (radius > outerRadius && selected_ >= 0 && menu_) {
					(*menu_)[highlightedItem].trigger();
					point_pose_.tracking(false);
			}
		}
	}
}

bool MenuController::leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame)
{
	progress_ = 0.0f;

	switch (leap_state_)
	{
	case LeapState::closed:
		leapMenuClosed(leapController, currentFrame);
		return true;
	case LeapState::triggered_context:
	case LeapState::triggered_main:
		leapMenuTriggered(leapController, currentFrame);
		break;
	case LeapState::open:
		leapMenuOpen(leapController, currentFrame);
		break;
	}

	return false;
}

void MenuController::update(std::chrono::milliseconds elapsed)
{
	transition_.update(elapsed);
}

void MenuController::draw()
{
	if (menu_) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		menuShader.enable();
		glUniformMatrix4fv(menuShader.getUniform("modelViewProjection"), 1, false, modelViewProjection * gl::scale(transition_.progress()));
		menuVBO.bind();
		menuIBO.bind();
		setShaderState();

		Vec3 menuC;
		Vec3 hlC;
		Vec3 tc;
		Vec3 tc2;
		if (MainController::getInstance().getRenderer().getBackgroundColor().x > 0.5f) {
			menuC = Vec3(.7f, .7f, .7f);
			hlC = Vec3(.1f, .1f, .1f);
			tc = Vec3(0, 0, 0);
			tc2 = Vec3(1.0f) - tc;
		} else {
			menuC = Vec3(.15f, .15f, .15f);
			hlC = Vec3(0.2f, 0.4f, 0.9f);
			tc = Vec3(1, 1, 1);
			tc2 = tc;
		}

		for (int i = 0; i < menu_->getItems().size(); i++) {

			if (i == selected_) {
				glUniformMatrix4fv(menuShader.getUniform("modelViewProjection"), 1, false, modelViewProjection * gl::scale(transition_.progress() + (0.2f * progress_)));
				Vec3 c = gl::lerp(menuC, Vec3(0.5f, 0.9f, 0.5f), progress_);
				glUniform4f(menuShader.getUniform("color"), c.x, c.y, c.z, transition_.progress());
			} else {
				glUniformMatrix4fv(menuShader.getUniform("modelViewProjection"), 1, false, modelViewProjection * gl::scale(transition_.progress()));
				glUniform4f(menuShader.getUniform("color"), menuC.x, menuC.y, menuC.z, transition_.progress());
			}

			void* offset = (void*)(indicesPerMenuItem * i * sizeof(GLushort));
			glDrawElements(GL_TRIANGLES, indicesPerMenuItem, indexType, offset);
		}
		//glUniform4f(menuShader.getUniform("color"), menuC.x, menuC.y, menuC.z, transition_.progress() * 0.85f);
		//glDrawElements(GL_TRIANGLES, indexCount, indexType, 0);

		//if (selected_ >= 0) {
		//	glUniformMatrix4fv(menuShader.getUniform("modelViewProjection"), 1, false, modelViewProjection * gl::scale(transition_.progress() + (0.2f * progress_)));
		//	Vec3 c = gl::lerp(Vec3(0.5f), Vec3(0.5f, 0.9f, 0.7f), progress_);
		//	glUniform4f(menuShader.getUniform("color"), c.x, c.y, c.z, transition_.progress());
		//	void* offset = (void*)(indicesPerMenuItem * selected_ * sizeof(GLushort));
		//	glDrawElements(GL_TRIANGLES, indicesPerMenuItem, indexType, offset);
		//}
		glDisable(GL_BLEND);

		if (transition_.full()) {
			drawMenu(*menu_, tc, tc2);
		}
	}

	// leap cursor
	if (leap_state_ != LeapState::closed) {
		Draw& d = MainController::getInstance().draw();
		d.setModelViewProj(gl::ortho2D(viewport_.x, viewport_.width, viewport_.y, viewport_.height));

		if (leap_state_ != LeapState::open) {
			Vec2 c = viewport_.center();
			d.color(.15f, .15f, .15f);
			d.begin(GL_TRIANGLE_FAN);
			d.circle(c.x, c.y, 40.0f, 32);
			d.end();
			d.draw();

			TextRenderer& text = MainController::getInstance().getText();
			text.begin(viewport_.width, viewport_.height);
			text.setColor(1.0f, 1.0f, 1.0f);
			text.add("Menu", c.x, c.y, TextRenderer::CENTER, TextRenderer::CENTER);
			text.end();
		}

		d.color(0, 0, 0);
		d.begin(GL_TRIANGLE_FAN);
		d.circle(leap.x, leap.y, 15.0f, 32);
		d.end();
		d.draw();
		d.color(1, 1, 1);
		d.begin(GL_TRIANGLE_FAN);
		d.circle(leap.x, leap.y, 13.0f, 32);
		d.end();
		d.draw();
	}
}


void MenuController::drawMenu(Menu& menu, Vec3 textColor1, Vec3 textColor2)
{
	TextRenderer& text = MainController::getInstance().getText();

	text.begin(viewport_.width, viewport_.height);

	text.setColor(textColor1.x, textColor1.y, textColor1.z);
	text.add(
		menu.getName(),
		viewport_.width / 2,
		viewport_.height,
		TextRenderer::CENTER,
		TextRenderer::TOP);

	double radius = std::min(viewport_.width, viewport_.height) * 0.5 * 0.7;
	double angleStep = gl::two_pi / menu.getItems().size();
	double angle = angleStep / 2.0;
	for (MenuItem& item : menu.getItems()) {
		int x = static_cast<int>(std::cos(angle) * radius + viewport_.width / 2);
		int y = static_cast<int>(std::sin(angle) * radius + viewport_.height / 2);
		text.add(item.getName(), x, y, TextRenderer::CENTER, TextRenderer::CENTER);
		angle += angleStep;
	}

	text.end();

	// draw highlighted
	if (selected_ >= 0 && selected_ < menu.getItems().size()) {
		double angle = angleStep * selected_ + angleStep / 2.0;
		int x = static_cast<int>(std::cos(angle) * radius + viewport_.width / 2);
		int y = static_cast<int>(std::sin(angle) * radius + viewport_.height / 2);

		text.setColor(textColor2.x, textColor2.y, textColor2.z);
		text.begin(viewport_.width, viewport_.height);
		text.add(menu.getItems()[selected_].getName(), x, y,
			TextRenderer::CENTER, TextRenderer::CENTER);
		text.end();
	}
}

void MenuController::createRingGeometry()
{
	if (!menu_)
		return;

	GLfloat innerRadius = min(viewport_.width, viewport_.height) * 0.5 * 0.55;
	GLfloat outerRadius = min(viewport_.width, viewport_.height) * 0.5 * 0.85;// 0.5 * sqrt(viewport_.width * viewport_.width + viewport_.height * viewport_.height);

	vector<GLfloat> verts;
	vector<GLushort> indices;

	std::function<void(float, float)> pushVert = [&](float angle, float radius){
		verts.push_back(cos(angle) * radius);
		verts.push_back(sin(angle) * radius);
	};

	std::function<void(GLushort, GLushort, GLushort)> pushTriangle = [&](GLushort i, GLushort j, GLushort k) {
		indices.push_back(i);
		indices.push_back(j);
		indices.push_back(k);
	};

	// I want a consistent number of segments for a smooth circle regardless of the number of menu items.
	// However, I also need the segments to align with the boundaries of the menu items.
	unsigned stepsPerItem = std::max(1u, static_cast<unsigned>(128u / menu_->getItems().size()));
	unsigned numSteps = stepsPerItem * menu_->getItems().size();
	this->indicesPerMenuItem = stepsPerItem * 6;

	unsigned jmod = 2 * numSteps;
	float step = two_pi / numSteps;
	float angle = 0.0f;
	for (unsigned i = 0; i < numSteps; i++) {
		pushVert(angle, innerRadius);
		pushVert(angle, outerRadius);
		int j = i * 2;
		pushTriangle(j, j + 1, (j + 3) % jmod);
		pushTriangle(j, (j + 3) % jmod, (j + 2) % jmod);
		angle += step;
	}

	menuVBO.bind();
	menuVBO.data(&verts[0], verts.size() * sizeof(GLfloat));

	menuIBO.bind();
	menuIBO.data(&indices[0], indices.size() * sizeof(GLushort));
	indexCount = indices.size();
	indexType = GL_UNSIGNED_SHORT;

	setShaderState = [this] {
		int loc = menuShader.getAttribute("vs_position");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 2, GL_FLOAT, false, 0, 0);
	};
}

void MenuController::createListGeometry()
{

}

void MenuController::resize()
{
	modelViewProjection = ortho2D(0, viewport_.width, 0, viewport_.height) * translation(viewport_.width / 2.0f, viewport_.height / 2.0f, 0);
	createRingGeometry();
}