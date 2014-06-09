#include "Transfer1DController.h"
#include "main/MainController.h"
#include "util/Util.h"
#include "Histogram.h"

using namespace gl;
using namespace std;
using namespace Leap;
using namespace std::chrono;

Transfer1DController::Transfer1DController()
{
    lMouseDrag = false;
    rMouseDrag = false;
	leap_drag_performed_ = false;
	selected_ = nullptr;
	dirty_textures_ = true;
    
	volumeRenderer = NULL;

	Transfer1D black_to_white;
	black_to_white.gradient(true);
	black_to_white.add(0.0f, 0.1f, { 0.f, 0.f, 0.f, 0.f });
	black_to_white.add(1.0f, 0.1f, { 1.f, 1.f, 1.f, 1.f });
	transfers_.push_back(black_to_white);

	Transfer1D warm;
	warm.gradient(true);
	warm.add(0.0f, 0.2f, { 0.f, 0.f, 0.f, 0.f });
	warm.add(0.2f, 0.2f, { 1.f, 0.f, 0.f, 0.1f });
	warm.add(0.8f, 0.2f, { 1.f, 1.f, 0.f, 0.8f });
	warm.add(1.0f, 0.2f, { 1.f, 1.f, 1.f, 1.0f });
	transfers_.push_back(warm);

	Transfer1D single_color;
	single_color.gradient(false);
	single_color.add(0.5f, 0.25f, { 1.0f, 1.0f, 1.0f, 0.5f });
	transfers_.push_back(single_color);

	histoProg = Program::create("shaders/tf1d_histo.vert", "shaders/tf1d_histo.frag");
	histoOutlineProg = Program::create("shaders/tf1d_histo_outline.vert", "shaders/tf1d_histo_outline.frag");

	// vertex buffer for geometry: contains vertices for
	// 1) the histogram quad (drawn as a texture)
	// 2) the cursor / value marker line
	GLfloat vertexData[] = {
		// start of texture quad vertices
		-1, -0.5, 0, 0,
		1, -0.5, 1, 0,
		1, 1, 1, 1,
		-1, -0.5, 0, 0,
		1, 1, 1, 1,
		-1, 1, 0, 1,
		// start of cursor line vertices
		-1, -1, 0, 0,
		-1, 1, 0, 0

	};
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	stride = 4 * sizeof(GLfloat);

	{
		bgShader = Program::create("shaders/menu.vert", "shaders/menu.frag");
		bgShader.enable();

		GLfloat vertices[] = {
			-1, -1,
			+1, -1,
			+1, +1,
			-1, -1,
			+1, +1,
			-1, +1
		};

		bgBuffer.generate(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
		bgBuffer.bind();
		bgBuffer.data(vertices, sizeof(vertices));
	}

	clutTexture.generate(GL_TEXTURE_1D);
	contextTexture.generate(GL_TEXTURE_1D);
	updateTextures();

	poses_.v().enabled(true);
	poses_.v().openFn([&](const Leap::Frame&){ leap_drag_performed_ = false; });
	poses_.v().disengageFunction([&](const Leap::Frame&){leap_drag_performed_ = false; });
	//poses_.v().closeFn([&](const Leap::Frame&) { changeColorSelected(); });

	poses_.pinch().enabled(true);
	poses_.pinch().closeFn([&](const Leap::Frame&){ toggleContextSelected(); });

	poses_.point2().enabled(true);
	poses_.point2().disengageOnExit(true);
	poses_.point2().exitSpeed(300.0f);
	poses_.point2().engageFunction([&](const Leap::Frame&) {
		saveInterval();
	});
}

void Transfer1DController::gainFocus()
{
	MainController::getInstance().showTransfer1D(true);
	auto& lsc = MainController::getInstance().leapStateController();
	lsc.clear();
	lsc.add(LeapStateController::icon_point_circle, "Main Menu");
	lsc.add(LeapStateController::icon_three_circle, "Options");
	lsc.add(LeapStateController::icon_point2, "Contrast");
    lsc.add(LeapStateController::icon_fist, "Rotate/Zoom");
    lsc.add(LeapStateController::icon_l_open, "Center");
    lsc.add(LeapStateController::icon_v_open, "Edit Color");
    lsc.add(LeapStateController::icon_pinch, "Toggle Context");
}

void Transfer1DController::loseFocus()
{
}

std::unique_ptr<Menu> Transfer1DController::contextMenu()
{
	Menu* menu = new Menu("Transfer 1D");
	MenuItem& mi_create = menu->createItem("New Function");
	mi_create.setAction([&]{
		createFunction();
		MainController::getInstance().menuController().hideMenu();
	});

	MenuItem& mi_delete = menu->createItem("Delete Function");
	mi_delete.setAction([&]{
		deleteFunction();
		MainController::getInstance().menuController().hideMenu();
	});

	MenuItem& mi_gradient = menu->createItem("Toggle Gradient");
	mi_gradient.setAction([&]{
		toggleGradient();
		MainController::getInstance().menuController().hideMenu();
	});

	return std::unique_ptr<Menu>(menu);
}

void Transfer1DController::setVolume(VolumeData* volume)
{
    this->volume = volume;
    
	// create histogram geometry
    
    // 512 bins is somewhat arbitrary; consider adding customization later
    int numBins = 512;
    Histogram histogram(volume->getMinValue(), volume->getMaxValue(), numBins);
    
    switch (volume->getType())
    {
        case GL_BYTE:
            histogram.readData((GLbyte*)volume->getData(), volume->getNumVoxels());
            break;
        case GL_UNSIGNED_BYTE:
			histogram.readData((GLubyte*)volume->getData(), volume->getNumVoxels());
            break;
        case GL_SHORT:
			histogram.readData((GLshort*)volume->getData(), volume->getNumVoxels());
            break;
        case GL_UNSIGNED_SHORT:
			histogram.readData((GLushort*)volume->getData(), volume->getNumVoxels());
            break;
    }
    
	double logMaxFreq = std::log(histogram.getMaxFrequency() + 1);

	// histo vbo triangle strip
	{
		vector<Vec2> buffer;
		for (unsigned i = 0; i < histogram.getNumBins(); ++i) {
			float x = (float)i / (histogram.getNumBins() - 1);
			float y = std::log(histogram.getSize(i) + 1) / logMaxFreq;
			x = (x - 0.5f) * 2.0f;
			y = (y - 0.5f) * 2.0f;
			buffer.push_back({ x, y });
			buffer.push_back({ x, -1.0f });
		}

		histoVBO.generateVBO(GL_STATIC_DRAW);
		histoVBO.bind();
		histoVBO.data(&buffer[0], buffer.size() * sizeof(Vec2));
		histoVBOCount = buffer.size();
	}
}

void Transfer1DController::setVolumeRenderer(VolumeController* volumeRenderer)
{
    this->volumeRenderer = volumeRenderer;
    volumeRenderer->setCLUTTexture(clutTexture);
	volumeRenderer->tex_context_ = contextTexture;
}

void Transfer1DController::setSliceRenderer(SliceController* sliceRenderer)
{
    this->sliceRenderer = sliceRenderer;
	sliceRenderer->setCLUTTexture(clutTexture);
}

bool Transfer1DController::keyboardInput(GLFWwindow* window, int key, int action, int mods)
{
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		nextCLUT();
    }

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		prevCLUT();
	}
    
	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		transfer().saveTexture(clutTexture);
		transfer().saveContext(contextTexture);
		volumeRenderer->markDirty();
	}

    return true;
}

bool Transfer1DController::mouseMotion(GLFWwindow* window, double x, double y)
{
    if (!viewport_.contains(x, y)) {
        return true;
    }
    
    if (lMouseDrag) {
		cursor_ = static_cast<float>((x - viewport_.x) / viewport_.width);
		chooseSelected();
		moveSelected();
    }
    
    return true;
}

bool Transfer1DController::mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y)
{
    lMouseDrag = button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS;
    rMouseDrag = button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS;
    

	// left click = select & move
	// middle click = create / delete
	// right click = select & widen


	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		// clut marker drag
		float center = static_cast<float>((x - viewport_.x) / viewport_.width);
		selected_ = transfer().closest(center);
	}

	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		float center = static_cast<float>((x - viewport_.x) / viewport_.width);
		selected_ = transfer().closest(center);

		auto cb = [&](const Color& color) {
			selected_->color(color);
			transfer().saveTexture(clutTexture);
			transfer().saveContext(contextTexture);
			volumeRenderer->markDirty();
		};

		MainController::getInstance().pickColor(selected_->color(), cb);
		return false;
	}
    
	return true;
}

bool Transfer1DController::leapInput(const Leap::Controller& leapController, const Leap::Frame& frame)
{
	poses_.update(frame);

	if (poses_.point2().tracking()) {
		scaleAllMarkers();
		return false;
	}

	if (poses_.v().tracking()) {
		cursor_ = frame.interactionBox().normalizePoint(poses_.v().handPosition(true)).x;

		if (poses_.v().isClosed()) {
			if (!leap_drag_performed_ && poses_.v().hand().palmVelocity().z < -250) {
				if (selected_) {
					changeColorSelected();
				} else {
					addMarker();
				}
			} else if (selected_ && !leap_drag_performed_ && poses_.v().hand().palmVelocity().z > 250) {
				deleteSelected();
			} else if (selected_ && !leap_drag_performed_) {
				moveSelected();

				Vector d = poses_.v().handPositionDelta(true);
				Vector hand_velocity = poses_.v().hand().palmVelocity();
				float delta_width = poses_.v().handPositionDelta(true).y / 200.0f;
				delta_width *= max(0.0f, 1.0f - hand_velocity.x * hand_velocity.x / 10000.0f);
				float width = selected_->width() + delta_width;
				scaleSelected(width);
			}
		} else {
			chooseSelected();
		}
	}

	if (!poses_.v().tracking()) {
		camera_control_.update(leapController, frame);
	}

	static auto lastSwipe = std::chrono::system_clock::now();
	Leap::GestureList gestures = frame.gestures();
	for (Leap::Gesture g : gestures) {
		if (g.type() == Leap::Gesture::TYPE_SWIPE) {

			auto curTime = std::chrono::system_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - lastSwipe);
			Leap::SwipeGesture swipe(g);

			if (elapsed.count() > 350 && g.isValid() && swipe.speed() > 300 && std::abs(swipe.direction().z) < 0.5f && swipe.hands().frontmost().fingers().extended().count() == 5) {
				lastSwipe = curTime;
				if (swipe.direction().x > 0)
					nextCLUT();
				else
					prevCLUT();
			}
		}
	}

	if (dirty_textures_) {
		updateTextures();
	}

	return false;
}

std::set<Leap::Gesture::Type> Transfer1DController::requiredGestures()
{
	std::set<Leap::Gesture::Type> gestures;
	gestures.insert(Leap::Gesture::TYPE_SWIPE);
	return gestures;
}

void Transfer1DController::draw()
{
	if (dirty_textures_) {
		updateTextures();
	}

	static const int totalHeight = 80;
	static const float colorBarHeight = 0.3f;
	static const int histoHeight = totalHeight;

	glViewport(viewport_.x, viewport_.y + viewport_.height * 0.2f, viewport_.width, viewport_.height * 0.8f);
	drawHistogram();
	glViewport(viewport_.x, viewport_.y, viewport_.width, viewport_.height * 0.2f);
	drawBackground();
	drawMarkerBar();

	viewport_.apply();

	// draw leap cursor
	Draw& d = MainController::getInstance().draw();
	d.setModelViewProj(viewport_.orthoProjection());
	d.begin(GL_LINES);
	d.color(1.0f, 0.0f, 0.0f);
	d.line(cursor_ * viewport_.width, viewport_.y, cursor_ * viewport_.width, viewport_.top());
	d.end();
	d.draw();
}

void Transfer1DController::drawMarkerBar()
{
	static Draw d;

	viewport_.apply();

	float outer_height = viewport_.height * 0.3f;
	float outer_side = 2.0f * outer_height / sqrt(3);
	float inner_height = outer_height * 0.75f;
	float inner_side = 2.0f * inner_height / sqrt(3);

	d.setModelViewProj(viewport_.orthoProjection());
	d.begin(GL_TRIANGLES);
	for (const Transfer1D::Marker& marker : transfer().markers()) {
		Vec3 c = marker.color().vec3();
		float x = viewport_.width * marker.center();

		// outer triangle with context highlight
		float half_side = outer_side * 0.5f;
		float l = x - half_side;
		float r = x + half_side;

		if (selected_ && selected_->center() == marker.center()) {
			if (marker.context()) {
				d.color(1.0f, 0.5f, 0.5f);
			} else {
				d.color(1.0f, 1.0f, 1.0f);
			}
		} else {
			if (marker.context()) {
				d.color(0.5f, 0.25f, 0.25f);
			} else {
				d.color(0.5f, 0.5f, 0.5f);
			}
		}

		d.vertex(l, 0.0f);
		d.vertex(x, outer_height);
		d.vertex(r, 0.0f);

		// inner triangle with marker color
		half_side = inner_side * 0.5f;
		l = x - half_side;
		r = x + half_side;
		d.color(c.x, c.y, c.z);
		d.vertex(l, 0.0f);
		d.vertex(x, inner_height);
		d.vertex(r, 0.0f);
	}
	d.end();
	d.draw();
}

void Transfer1DController::drawBackground()
{
	bgBuffer.bind();
	bgShader.enable();
	glUniform4f(bgShader.getUniform("color"), 0.0f, 0.0f, 0.0f, 1.0f);
	glUniformMatrix4fv(bgShader.getUniform("modelViewProjection"), 1, false, Mat4());

	GLint loc = bgShader.getAttribute("vs_position");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, false, 2 * sizeof(GLfloat), 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Transfer1DController::drawHistogram()
{
	clutTexture.bind();
	histoVBO.bind();
	histoProg.enable();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, histoVBOCount);

	histoOutlineProg.enable();
	histoOutlineProg.uniform("color", 0.5f, 0.5f, 0.5f, 1.0f);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 4 * sizeof(GLfloat), 0);
	glDrawArrays(GL_LINE_STRIP, 0, histoVBOCount / 2);
}

Transfer1D& Transfer1DController::transfer()
{
	return transfers_[active_transfer_];
}

void Transfer1DController::nextCLUT()
{
	if (transfers_.size() > 1) {
		active_transfer_++;
		if (active_transfer_ == transfers_.size()) {
			active_transfer_ = 0;
		}
		markDirty();
	}
}

void Transfer1DController::prevCLUT()
{
	if (transfers_.size() > 1) {
		active_transfer_--;
		if (active_transfer_ < 0) {
			active_transfer_ = transfers_.size() - 1;
		}
		markDirty();
	}
}

void Transfer1DController::markDirty()
{
	dirty_textures_ = true;
	volumeRenderer->markDirty();
}

void Transfer1DController::addMarker()
{
    auto& lsc = MainController::getInstance().leapStateController();
    lsc.increaseBrightness(LeapStateController::icon_v_open);
	leap_drag_performed_ = true;
	Vec3 c = Vec3::random();
	transfer().add(cursor_, 0.2f, { c.x, c.y, c.z, 1.0f });
	markDirty();
}

void Transfer1DController::updateTextures()
{
	transfer().saveTexture(clutTexture);
	transfer().saveContext(contextTexture);
	dirty_textures_ = false;
}

void Transfer1DController::chooseSelected()
{
	selected_ = transfer().closest(cursor_);
	if (abs(selected_->center() - cursor_) > 0.1f) {
		selected_ = nullptr;
	}
}

void Transfer1DController::moveSelected()
{
	if (selected_) {
        auto& lsc = MainController::getInstance().leapStateController();
        lsc.increaseBrightness(LeapStateController::icon_v_open);
		selected_->center(cursor_);
		markDirty();
	}
}

void Transfer1DController::scaleSelected(float width)
{
	if (selected_) {
        auto& lsc = MainController::getInstance().leapStateController();
        lsc.increaseBrightness(LeapStateController::icon_v_open);
		selected_->width(width);
		markDirty();
	}
}

void Transfer1DController::deleteSelected()
{
	if (selected_ && transfer().markers().size() > 1) {
        auto& lsc = MainController::getInstance().leapStateController();
        lsc.increaseBrightness(LeapStateController::icon_v_open);
		leap_drag_performed_ = true;
		transfer().remove(selected_->center());
		markDirty();
	}
}

void Transfer1DController::changeColorSelected()
{
	leap_drag_performed_ = true;

	auto cb = [&](const Color& color) {
        auto& lsc = MainController::getInstance().leapStateController();
        lsc.increaseBrightness(LeapStateController::icon_v_open);
		selected_->color(color);
		transfer().saveTexture(clutTexture);
		transfer().saveContext(contextTexture);
		volumeRenderer->markDirty();
	};
	MainController::getInstance().pickColor(selected_->color(), cb);
}

void Transfer1DController::toggleContextSelected()
{
	chooseSelected();
	if (selected_) {
        auto& lsc = MainController::getInstance().leapStateController();
        lsc.increaseBrightness(LeapStateController::icon_pinch);
		selected_->context(!selected_->context());
		markDirty();
	}
}

void Transfer1DController::saveInterval()
{
	saved_interval_.center(transfer().center());
	saved_interval_.width(transfer().width());
	saved_centers_.clear();
	for (const Transfer1D::Marker& m : transfer().markers()) {
		saved_centers_.push_back(m.center());
	}
}

void Transfer1DController::scaleAllMarkers()
{
	if (saved_centers_.size() > 1) {
        auto& lsc = MainController::getInstance().leapStateController();
        lsc.increaseBrightness(LeapStateController::icon_point2);
		float dl = poses_.point2().leftPointer().stabilizedTipPosition().x - poses_.point2().leftPointerEngaged().stabilizedTipPosition().x;
		float dr = poses_.point2().rightPointer().stabilizedTipPosition().x - poses_.point2().rightPointerEngaged().stabilizedTipPosition().x;

		// map from old interval [a,b] to new interval [c,d]
		float a = saved_centers_.front();
		float b = saved_centers_.back();
		float c = a + dl / 400.0f;
		float d = b + dr / 400.0f;
		float s = (d - c) / (b - a);
		vector<float> centers;
		for (float p : saved_centers_) {
			centers.push_back((p - a) * s + c);
		}

		transfer().move(centers);
		markDirty();
	}
}

void Transfer1DController::createFunction()
{
	Transfer1D t;
	t.add(0.5f, 0.2f, { 0.5f, 0.5f, 0.5f, 0.5f });
	transfers_.push_back(t);
	active_transfer_ = transfers_.size() - 1;
	markDirty();
}

void Transfer1DController::deleteFunction()
{
	if (transfers_.size() > 1) {
		transfers_.erase(transfers_.begin() + active_transfer_);
		active_transfer_ = transfers_.size() - 1;
		if (active_transfer_ < 0) {
			active_transfer_ = 0;
		}
		markDirty();
	}
}

void Transfer1DController::toggleGradient()
{
	if (transfer().markers().size() > 1) {
		transfer().gradient(!transfer().gradient());
		markDirty();
	}
}