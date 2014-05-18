#include "Transfer1DController.h"
#include "main/MainController.h"

using namespace gl;
using namespace std;

Transfer1DController::Transfer1DController() : histogram(NULL), transfer1DPixels(NULL)
{
    lMouseDrag = false;
    rMouseDrag = false;
	leap_drag_performed_ = false;
	selected_ = nullptr;
    
	volumeRenderer = NULL;

	// start out with the default CLUT: black->white gradient
	{
		CLUT c(CLUT::continuous);
		c.addMarker({ { 0.0f, 0.2f }, { 0.f, 0.f, 0.f, 0.f } });
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
		c.addMarker({ { 0.2f, 0.2f }, { 0.0f, 0.0f, 1.0f, 0.5f }, false });
		c.addMarker({ { 0.4f, 0.2f }, { 0.0f, 0.0f, 1.0f, 0.5f }, true });
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
	cluts[activeCLUT].saveTexture(clutTexture);
	cluts[activeCLUT].saveContext(contextTexture);

	pinch_pose_.openFn([&](const Leap::Frame&){leap_drag_performed_ = false; });
	pinch_pose_.disengageFunction([&](const Leap::Frame&){leap_drag_performed_ = false; });
	pinch_pose_.closeFn([&](const Leap::Frame&){
		static std::chrono::high_resolution_clock::time_point last_close = std::chrono::high_resolution_clock::now();

		auto now = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_close);
		last_close = now;
		cout << elapsed.count() << endl;

		if (pinch_pose_.isPinching()) {
			selected_ = cluts[activeCLUT].closestMarker(cursor_center_);
			if (abs(selected_->interval().center() - cursor_center_) < 0.1f) {
				saved_interval_ = selected_->interval();
			} else {
				selected_ = nullptr;
			}
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
				l_pose_.tracking(false);
			};
			MainController::getInstance().pickColor(selected_->color(), cb);
		}
	});

	l_pose_.minValidFrames(5);
	l_pose_.openFn([&](const Leap::Frame&){leap_drag_performed_ = false; });
	l_pose_.disengageFunction([&](const Leap::Frame&){leap_drag_performed_ = false; });
	l_pose_.clickFn([&](const Leap::Frame&) {
		selected_ = cluts[activeCLUT].closestMarker(cursor_center_);
		auto cb = [&](const Color& color) {
			selected_->color(color);
			cluts[activeCLUT].saveTexture(clutTexture);
			cluts[activeCLUT].saveContext(contextTexture);
			volumeRenderer->markDirty();
			l_pose_.tracking(false);
		};
		MainController::getInstance().pickColor(selected_->color(), cb);
	});

	point_2_pose_.disengageOnExit(true);
	point_2_pose_.exitSpeed(300.0f);
	point_2_pose_.engageFunction([&](const Leap::Frame&){
		saved_interval_ = cluts[activeCLUT].interval();
	});
}

Transfer1DController::~Transfer1DController()
{
    if (transfer1DPixels)
        delete transfer1DPixels;
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
    
    if (histogram)
        delete histogram;
    
    // 512 bins is somewhat arbitrary; consider adding customization later
    int numBins = 512;
    histogram = new Histogram(volume->getMinValue(), volume->getMaxValue(), numBins);
    
    switch (volume->getType())
    {
        case GL_BYTE:
            histogram->readData((GLbyte*)volume->getData(), volume->getNumVoxels());
            break;
        case GL_UNSIGNED_BYTE:
            histogram->readData((GLubyte*)volume->getData(), volume->getNumVoxels());
            break;
        case GL_SHORT:
            histogram->readData((GLshort*)volume->getData(), volume->getNumVoxels());
            break;
        case GL_UNSIGNED_SHORT:
            histogram->readData((GLushort*)volume->getData(), volume->getNumVoxels());
            break;
    }
    
	this->histogram = histogram;
	//int drawWidth = histogram->getNumBins();
	//int drawHeight = 256;

	//std::vector<unsigned char> pixels(drawWidth * drawHeight);
	//std::fill(pixels.begin(), pixels.end(), 0);

	double logMaxFreq = std::log(histogram->getMaxFrequency() + 1);

	//for (int bin = 0; bin < histogram->getNumBins(); bin++) {
	//	int size = histogram->getSize(bin);
	//	double sizeNorm = std::log(size + 1) / logMaxFreq;

	//	int binHeight = (int)(sizeNorm * drawHeight);

	//	for (int j = 0; j < binHeight; j++) {
	//		pixels[bin + j * drawWidth] = 255;
	//	}
	//}

	//histo1D.bind();
	//histo1D.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//histo1D.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//histo1D.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//histo1D.setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//histo1D.setData2D(GL_RED, drawWidth, drawHeight, GL_RED, GL_UNSIGNED_BYTE, &pixels[0]);


	// histo vbo triangle strip
	{
		vector<Vec2> buffer;
		for (unsigned i = 0; i < histogram->getNumBins(); ++i) {
			float x = (float)i / (histogram->getNumBins() - 1);
			float y = std::log(histogram->getSize(i) + 1) / logMaxFreq;
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
    
    if (transfer1DPixels)
        delete transfer1DPixels;
    transfer1DPixels = new GLubyte[histogram->getNumBins() * 256]; // 256 is height (shouldn't be hardcoded)
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
	using namespace Leap;

	point_2_pose_.update(frame);
	if (point_2_pose_.tracking()) {
		float c = saved_interval_.center() + point_2_pose_.handsCenterDeltaEngaged(true).x / 400.0f;
		cluts[activeCLUT].interval().center(c);

		float delta_width = point_2_pose_.handsSeparationDeltaEngaged(true);
		float w = std::max(0.0f, saved_interval_.width() + delta_width / 400.0f);
		cluts[activeCLUT].interval().width(w);

		cluts[activeCLUT].saveTexture(clutTexture);
		cluts[activeCLUT].saveContext(contextTexture);
		volumeRenderer->markDirty();
		return false;
	}

	//l_pose_.update(frame);
	//if (l_pose_.tracking()) {
	//	InteractionBox ib = frame.interactionBox();
	//	Vector v = ib.normalizePoint(l_pose_.pointer().stabilizedTipPosition());
	//	cursor_center_ = v.x;
	//	leap_cursor_ = Vec2(v.x, v.y) * Vec2(viewport_.width, viewport_.height) + Vec2(viewport_.x, viewport_.y);

	//	if (l_pose_.isClosed() && !leap_drag_performed_) {
	//		Vector dp = l_pose_.pointer().tipPosition() - l_pose_.pointerClosed().tipPosition();
	//		if (dp.y > 35) {
	//			leap_drag_performed_ = true;
	//			Interval interval(cursor_center_, 0.2f);
	//			ColorRGB marker_color{ .5f, .5f, .5f, 1.0f };
	//			cluts[activeCLUT].addMarker(CLUT::Marker({ interval, marker_color }));
	//			cluts[activeCLUT].saveTexture(clutTexture);
	//			volumeRenderer->markDirty();
	//		} else if (dp.y < -35) {
	//			if (!cluts[activeCLUT].markers().empty()) {
	//				leap_drag_performed_ = true;
	//				cluts[activeCLUT].removeMarker(cursor_center_);
	//				cluts[activeCLUT].saveTexture(clutTexture);
	//				volumeRenderer->markDirty();
	//			}
	//		}
	//	}

	//	return false;
	//}

	pinch_pose_.update(frame);
	if (pinch_pose_.tracking()) {
		InteractionBox ib = frame.interactionBox();

		Vector v = ib.normalizePoint(pinch_pose_.hand().stabilizedPalmPosition());

		cursor_center_ = v.x;
		leap_cursor_ = Vec2(v.x, v.y) * Vec2(viewport_.width, viewport_.height) + Vec2(viewport_.x, viewport_.y);

		if (pinch_pose_.isPinching()) {
			if (!leap_drag_performed_ && pinch_pose_.hand().palmVelocity().z < -250 && !selected_) {
				leap_drag_performed_ = true;
				Interval interval(cursor_center_, 0.2f);
				Vec3 c = Vec3::random();
				ColorRGB marker_color{ c.x, c.y, c.z, 1.0f };
				cluts[activeCLUT].addMarker(CLUT::Marker({ interval, marker_color }));
				cluts[activeCLUT].saveTexture(clutTexture);
				cluts[activeCLUT].saveContext(contextTexture);
				volumeRenderer->markDirty();
			} else if (!leap_drag_performed_ && pinch_pose_.hand().palmVelocity().z > 250 && selected_ && !cluts[activeCLUT].markers().empty()) {
				leap_drag_performed_ = true;
				cluts[activeCLUT].removeMarker(selected_->interval().center());
				cluts[activeCLUT].saveTexture(clutTexture);
				cluts[activeCLUT].saveContext(contextTexture);
				volumeRenderer->markDirty();
			} else if (selected_ && !leap_drag_performed_){
				Vector l = ib.normalizePoint(pinch_pose_.hand().stabilizedPalmPosition()) - ib.normalizePoint(pinch_pose_.handPinched().stabilizedPalmPosition());
				float width = saved_interval_.width() + l.y;
				if (pinch_pose_.isPinching()) {
					editInterval(cursor_center_, width);
				}
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

void Transfer1DController::editInterval(float center, float width)
{
	selected_->interval({ center, width });
	cluts[activeCLUT].saveTexture(clutTexture);
	cluts[activeCLUT].saveContext(contextTexture);
	volumeRenderer->markDirty();
}

void Transfer1DController::moveAndScale(const Leap::Controller& controller)
{
	//float c = saved_interval_.center() + finger_tracker_.centerDelta().x / 400.0f;
	//cluts[activeCLUT].interval().center(c);

	//float w = std::max(0.0f, saved_interval_.width() + finger_tracker_.fingerGapDelta() / 400.0f);
	//cluts[activeCLUT].interval().width(w);

	//cluts[activeCLUT].saveTexture(clutTexture);
	//volumeRenderer->markDirty();
}

std::set<Leap::Gesture::Type> Transfer1DController::requiredGestures()
{
	std::set<Leap::Gesture::Type> gestures;
	gestures.insert(Leap::Gesture::TYPE_SWIPE);
	return gestures;
}

void Transfer1DController::nextCLUT()
{
	if (++activeCLUT == cluts.size())
		activeCLUT = 0;
	cluts[activeCLUT].saveTexture(clutTexture);
	cluts[activeCLUT].saveContext(contextTexture);
	volumeRenderer->markDirty();
}

void Transfer1DController::prevCLUT()
{
	if (--activeCLUT < 0)
		activeCLUT = cluts.size() - 1;
	cluts[activeCLUT].saveTexture(clutTexture);
	cluts[activeCLUT].saveContext(contextTexture);
	volumeRenderer->markDirty();
}

void Transfer1DController::draw()
{
	static const int totalHeight = 80;
	static const float colorBarHeight = 0.3f;
	static const int histoHeight = totalHeight;

	glViewport(viewport_.x, viewport_.y + viewport_.height * 0.2f, viewport_.width, viewport_.height * 0.8f);
	drawHistogram();
	glViewport(viewport_.x, viewport_.y, viewport_.width, viewport_.height * 0.2f);
	drawBackground();
	drawMarkerBar();

	viewport_.apply();

	if (pinch_pose_.tracking() || l_pose_.tracking()) {
		Draw& d = MainController::getInstance().draw();
		d.setModelViewProj(viewport_.orthoProjection());
		d.begin(GL_LINES);
		d.color(1.0f, 0.0f, 0.0f);
		d.line(cursor_center_ * viewport_.width + viewport_.x, viewport_.y, cursor_center_ * viewport_.width + viewport_.x, viewport_.top());
		d.end();
		d.draw();
	}
}

void Transfer1DController::drawMarkerBar()
{
	static Draw d;
	d.begin(GL_TRIANGLES);
	for (const CLUT::Marker& marker : cluts[activeCLUT].markers()) {
		Vec3 c = marker.color().vec3();
		float x = marker.interval().center();

		if (cluts[activeCLUT].mode() == CLUT::continuous) {
			x = x * cluts[activeCLUT].interval().width() + cluts[activeCLUT].interval().left();
		}

		float max_scale = 1.5f;
		float scale_cutoff = 0.2f;
		float dist = abs(x - cursor_center_);
		float scale = max(1.0f, max_scale - dist / scale_cutoff);
		

		// [0,1] to [-1,1]
		x = (x - 0.5f) * 2.0f;

		float l = x - 0.05f * 800.0f / viewport_.width;
		float r = x + 0.05f * 800.0f / viewport_.width;
		d.color(.5f, .5f, .5f);
		d.vertex(l, -1);
		d.vertex(x, .6f);
		d.vertex(r, -1);

		d.color(c.x, c.y, c.z);
		l = x - 0.04f * 800.0f / viewport_.width;
		r = x + 0.04f * 800.0f / viewport_.width;
		d.vertex(l, -1);
		d.vertex(x, .5f);
		d.vertex(r, -1);
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
	contextTexture.bind();
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