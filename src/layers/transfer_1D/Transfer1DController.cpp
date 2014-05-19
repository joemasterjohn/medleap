#include "Transfer1DController.h"
#include "main/MainController.h"
#include "util/Util.h"
#include "Histogram.h"

using namespace gl;
using namespace std;
using namespace Leap;

Transfer1DController::Transfer1DController()
{
    lMouseDrag = false;
    rMouseDrag = false;
	leap_drag_performed_ = false;
	selected_ = nullptr;
	dirty_textures_ = true;
    
	volumeRenderer = NULL;

	// start out with the default CLUT: black->white gradient
	{
		CLUT c(CLUT::continuous);
		c.addMarker({ { 0.0f, 0.2f }, { 0.f, 0.f, 0.f, 0.f }, true });
		c.addMarker({ { 1.0f, 0.2f }, { 1.f, 1.f, 1.f, 1.f } });
		cluts.push_back(c);
	}

	{
		CLUT c(CLUT::continuous);
		c.addMarker({ { 0.0f, 0.2f }, { 0.f, 0.f, 0.f, 0.f } });
		c.addMarker({ { 0.2f, 0.2f }, { 1.f, 0.f, 0.f, 0.1f } });
		c.addMarker({ { 0.8f, 0.2f }, { 1.f, 1.f, 0.f, 0.8f } });
		c.addMarker({ { 1.0f, 0.2f }, { 1.f, 1.f, 1.f, 1.0f } });
		cluts.push_back(c);
	}

	{
		CLUT c(CLUT::piecewise);
		c.addMarker({ { 0.2f, 0.2f }, { 1.0f, 0.0f, 0.0f, 0.5f }, false });
		c.addMarker({ { 0.4f, 0.2f }, { 0.0f, 1.0f, 0.0f, 0.5f }, true });
		c.addMarker({ { 0.6f, 0.2f }, { 0.0f, 0.0f, 1.0f, 0.5f }, false });
		cluts.push_back(c);
	}
    
	{
		CLUT c(CLUT::piecewise);
		c.addMarker({ { 0.5f, 0.6f }, { 1.f, 1.f, 1.f, 0.5f } });
		cluts.push_back(c);
	}

	histo1D.generate(GL_TEXTURE_2D);
	transferFn.generate(GL_TEXTURE_2D);

	shader = Program::create("shaders/tf1d_histo.vert", "shaders/tf1d_histo.frag");
	histoProg = Program::create("shaders/tf1d_histo.vert", "shaders/tf1d_histo.frag");
	histoOutlineProg = Program::create("shaders/tf1d_histo_outline.vert", "shaders/tf1d_histo_outline.frag");
	colorShader = Program::create("shaders/histo_line.vert", "shaders/histo_line.frag");

	shader.enable();
	glUniform1i(shader.getUniform("tex_histogram"), 0);
	glUniform1i(shader.getUniform("tex_transfer"), 1);

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
	poses_.v().openFn([&](const Leap::Frame&){leap_drag_performed_ = false; });
	poses_.v().disengageFunction([&](const Leap::Frame&){leap_drag_performed_ = false; });
	poses_.v().closeFn([&](const Leap::Frame&) {
		static std::chrono::high_resolution_clock::time_point last_close = std::chrono::high_resolution_clock::now();

		auto now = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_close);
		last_close = now;

		selected_ = cluts[activeCLUT].closestMarker(cursor_center_);
		if (abs(selected_->interval().center() - cursor_center_) < 0.1f) {
			saved_interval_ = selected_->interval();
		} else {
			selected_ = nullptr;
		}

		if (elapsed.count() > 100 && elapsed.count() < 500 && !leap_drag_performed_) {
			selected_ = cluts[activeCLUT].closestMarker(cursor_center_);
			auto cb = [&](const Color& color) {
				selected_->color(color);
				cluts[activeCLUT].saveTexture(clutTexture);
				cluts[activeCLUT].saveContext(contextTexture);
				volumeRenderer->markDirty();
			};
			MainController::getInstance().pickColor(selected_->color(), cb);
		}
	});

	poses_.pinch().enabled(true);
	poses_.pinch().closeFn([&](const Leap::Frame&){
		// toggle context
		selected_ = cluts[activeCLUT].closestMarker(cursor_center_);
		if (selected_) {
			selected_->context(!selected_->context());
			cluts[activeCLUT].saveContext(contextTexture);
			volumeRenderer->markDirty();
		}
	});

	poses_.point2().enabled(true);
	poses_.point2().disengageOnExit(true);
	poses_.point2().exitSpeed(300.0f);
	poses_.point2().engageFunction([&](const Leap::Frame&){
		saved_interval_ = cluts[activeCLUT].interval();
	});
}

void Transfer1DController::gainFocus()
{
	MainController::getInstance().showTransfer1D(true);
	auto& lsc = MainController::getInstance().leapStateController();
	lsc.clear();
	lsc.add(LeapStateController::icon_h1f1_circle, "Main Menu");
	lsc.add(LeapStateController::icon_h1f2_circle, "Options");
	lsc.add(LeapStateController::icon_h2f1_point, "Windowing");
}

void Transfer1DController::loseFocus()
{
}

std::unique_ptr<Menu> Transfer1DController::contextMenu()
{
	Menu* menu = new Menu("Transfer 1D");
	menu->createItem("Create CLUT", []{MainController::getInstance().menuController().hideMenu(); });
	menu->createItem("Delete CLUT", []{MainController::getInstance().menuController().hideMenu(); });

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
		cluts[activeCLUT].saveTexture(clutTexture);
		cluts[activeCLUT].saveContext(contextTexture);
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

		if (cluts[activeCLUT].mode() == CLUT::continuous) {
			cluts[activeCLUT].interval().center(static_cast<float>(x - viewport_.x) / viewport_.width);
			cluts[activeCLUT].saveTexture(clutTexture);
			cluts[activeCLUT].saveContext(contextTexture);
			volumeRenderer->markDirty();
		}
		else if (selected_) {
			const Interval& current = selected_->interval();
			float newCenter = static_cast<float>((x - viewport_.x) / viewport_.width);
			selected_->interval({ newCenter, current.width() });
			cluts[activeCLUT].saveTexture(clutTexture);
			cluts[activeCLUT].saveContext(contextTexture);
			volumeRenderer->markDirty();
		}


    } else if (rMouseDrag) {
		Interval& intv = cluts[activeCLUT].interval();
		float cv = static_cast<float>(x) / viewport_.width;
		float cc = intv.center();
		intv.width(2.0f * std::abs(cv - cc));
		cluts[activeCLUT].saveTexture(clutTexture);
		cluts[activeCLUT].saveContext(contextTexture);
		volumeRenderer->markDirty();
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
		selected_ = cluts[activeCLUT].closestMarker(center);
	}

	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		float center = static_cast<float>((x - viewport_.x) / viewport_.width);
		selected_ = cluts[activeCLUT].closestMarker(center);

		auto cb = [&](const Color& color) {
			selected_->color(color);
			cluts[activeCLUT].saveTexture(clutTexture);
			cluts[activeCLUT].saveContext(contextTexture);
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
		// scale clut interval
		float c = saved_interval_.center() + poses_.point2().handsCenterDeltaEngaged(true).x / 400.0f;
		cluts[activeCLUT].interval().center(c);

		float delta_width = poses_.point2().handsSeparationDeltaEngaged(true);
		float w = std::max(0.0f, saved_interval_.width() + delta_width / 400.0f);
		cluts[activeCLUT].interval().width(w);

		markDirty();
		return false;
	}

	if (poses_.v().tracking()) {
		cursor_center_ = frame.interactionBox().normalizePoint(poses_.v().handPosition(true)).x;
		leap_cursor_ = denormalize(viewport_, frame, poses_.v().handPosition(true));

		if (poses_.v().isClosed() && !leap_drag_performed_) {

			if (poses_.v().hand().palmVelocity().z < -250 && !selected_) {
				addMarker();
			} else if (poses_.v().hand().palmVelocity().z > 250 && selected_ && !cluts[activeCLUT].markers().empty()) {
				deleteMarker();
			} else if (selected_) {
				Vector d = poses_.v().handPositionDelta(true);
				Vector hand_velocity = poses_.v().hand().palmVelocity();
				float delta_width = poses_.v().handPositionDelta(true).y / 200.0f;
				delta_width *= max(0.0f, 1.0f - hand_velocity.x * hand_velocity.x / 10000.0f);
				float width = selected_->interval().width() + delta_width;
				moveSelected();
				scaleSelected(width);
			}
		}
	}

	camera_control_.update(leapController, frame);

	static auto lastSwipe = std::chrono::system_clock::now();
	Leap::GestureList gestures = frame.gestures();
	for (Leap::Gesture g : gestures) {
		if (g.type() == Leap::Gesture::TYPE_SWIPE) {

			auto curTime = std::chrono::system_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - lastSwipe);
			Leap::SwipeGesture swipe(g);

			if (elapsed.count() > 350 && g.isValid() && swipe.speed() > 200 && std::abs(swipe.direction().z) < 0.5f && swipe.hands().frontmost().fingers().extended().count() == 5) {
				lastSwipe = curTime;
				if (swipe.direction().x > 0)
					nextCLUT();
				else
					prevCLUT();
			}
		}
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
		d.line(cursor_center_ * viewport_.width + viewport_.x, viewport_.y, cursor_center_ * viewport_.width + viewport_.x, viewport_.top());
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
	for (const CLUT::Marker& marker : cluts[activeCLUT].markers()) {
		Vec3 c = marker.color().vec3();

		float x;
		if (cluts[activeCLUT].mode() == CLUT::continuous) {
			x = marker.interval().center() * cluts[activeCLUT].interval().width() + cluts[activeCLUT].interval().left();
			x = viewport_.x + viewport_.width * x;
		} else {
			x = viewport_.x + viewport_.width * marker.interval().center();
		}

		float scale = 1.0f;

		// outer triangle with context highlight
		float half_side = outer_side * scale * 0.5f;
		float l = x - half_side;
		float r = x + half_side;
		if (marker.context()) {
			d.color(.25f, .25f, .25f);
		} else {
			d.color(1.0f, 1.0f, 1.0f);
		}
		d.vertex(l, 0.0f);
		d.vertex(x, outer_height * scale);
		d.vertex(r, 0.0f);

		// inner triangle with marker color
		half_side = inner_side * scale * 0.5f;
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

CLUT& Transfer1DController::currentCLUT()
{
	return cluts[activeCLUT];
}

void Transfer1DController::nextCLUT()
{
	if (cluts.size() > 1) {
		activeCLUT++;
		if (activeCLUT == cluts.size()) {
			activeCLUT = 0;
		}
		markDirty();
	}
}

void Transfer1DController::prevCLUT()
{
	if (cluts.size() > 1) {
		activeCLUT--;
		if (activeCLUT < 0) {
			activeCLUT = cluts.size() - 1;
		}
		markDirty();
	}
}

void Transfer1DController::updateTextures()
{
	currentCLUT().saveTexture(clutTexture);
	currentCLUT().saveContext(contextTexture);
	dirty_textures_ = false;
}

void Transfer1DController::markDirty()
{
	dirty_textures_ = true;
	volumeRenderer->markDirty();
}

void Transfer1DController::updateSelected(float cursor)
{
	selected_ = cluts[activeCLUT].closestMarker(cursor);
}

void Transfer1DController::toggleSelectedContext()
{
	if (selected_) {
		selected_->context(!selected_->context());
		markDirty();
	}
}

void Transfer1DController::moveSelected()
{
	if (selected_) {
		selected_->interval({ cursor_center_, selected_->interval().width() });
		markDirty();
	}
}

void Transfer1DController::scaleSelected(float width)
{
	if (selected_) {
		selected_->interval({ selected_->interval().center(), width });
		markDirty();
	}
}

void Transfer1DController::addMarker()
{
	leap_drag_performed_ = true;
	Vec3 c = Vec3::random();
	currentCLUT().addMarker(CLUT::Marker({ { cursor_center_, 0.2f }, { c.x, c.y, c.z, 1.0f } }));
	markDirty();
}

void Transfer1DController::deleteMarker()
{
	if (selected_) {
		leap_drag_performed_ = true;
		cluts[activeCLUT].removeMarker(selected_->interval().center());
		markDirty();
	}
}