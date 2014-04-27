#include "Transfer1DController.h"
#include "main/MainController.h"

using namespace gl;

Transfer1DController::Transfer1DController() : histogram(NULL), transfer1DPixels(NULL)
{
    lMouseDrag = false;
    rMouseDrag = false;
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
		c.addMarker({ { 0.0f, 0.2f }, { 1.f, 0.f, 0.f, 0.5f } });
		c.addMarker({ { 0.5f, 0.6f }, { 0.f, 1.f, 0.f, 0.5f } });
		c.addMarker({ { 1.0f, 0.2f }, { 0.f, 0.f, 1.f, 0.5f } });
		cluts.push_back(c);
	}
    
	{
		CLUT c(CLUT::piecewise);
		c.addMarker({ { 0.5f, 0.6f }, { 1.f, 1.f, 1.f, 0.5f } });
		cluts.push_back(c);
	}

    renderer.setCLUT(&cluts[activeCLUT = 0]);

	finger_tracker_.trackFunction(std::bind(&Transfer1DController::moveAndScale, this, std::placeholders::_1));
	finger_tracker_.engageFunction([&](const Leap::Controller&){saved_interval_ = cluts[activeCLUT].interval(); });

}

Transfer1DController::~Transfer1DController()
{
    if (transfer1DPixels)
        delete transfer1DPixels;
}

Transfer1DRenderer* Transfer1DController::getRenderer()
{
    return &renderer;
}

std::unique_ptr<Menu> Transfer1DController::contextMenu()
{
	Menu* menu = new Menu("Transfer 1D");
	menu->createItem("Create Color", []{MainController::getInstance().menuController().hideMenu(); });
	menu->createItem("Delete Color", []{MainController::getInstance().menuController().hideMenu(); });
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
    
    renderer.setHistogram(histogram);
    renderer.setVolume(volume);
    
    if (transfer1DPixels)
        delete transfer1DPixels;
    transfer1DPixels = new GLubyte[histogram->getNumBins() * 256]; // 256 is height (shouldn't be hardcoded)
}

void Transfer1DController::setVolumeRenderer(VolumeRenderer* volumeRenderer)
{
    this->volumeRenderer = volumeRenderer;
    volumeRenderer->setCLUTTexture(renderer.getCLUTTexture());
}

void Transfer1DController::setSliceRenderer(SliceRenderer* sliceRenderer)
{
    this->sliceRenderer = sliceRenderer;
    sliceRenderer->setCLUTTexture(renderer.getCLUTTexture());
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
		cluts[activeCLUT].saveTexture(renderer.getCLUTTexture());
		volumeRenderer->markDirty();
	}

    return true;
}

bool Transfer1DController::mouseMotion(GLFWwindow* window, double x, double y)
{
    if (!renderer.getViewport().contains(x, y)) {
        return true;
    }
    
    
    renderer.setCursor(static_cast<int>(x), static_cast<int>(y));
    
    if (lMouseDrag) {

		if (cluts[activeCLUT].mode() == CLUT::continuous) {
			cluts[activeCLUT].interval().center(static_cast<float>(x) / renderer.getViewport().width);
			cluts[activeCLUT].saveTexture(renderer.getCLUTTexture());
			volumeRenderer->markDirty();
		}
		else if (selected_) {
			const Interval& current = selected_->interval();
			float newCenter = static_cast<float>(x / renderer.getViewport().width);
			selected_->interval({ newCenter, current.width() });
			cluts[activeCLUT].saveTexture(renderer.getCLUTTexture());
			volumeRenderer->markDirty();
		}


    } else if (rMouseDrag) {
		Interval& intv = cluts[activeCLUT].interval();
		float cv = static_cast<float>(x) / renderer.getViewport().width;
		float cc = intv.center();
		intv.width(2.0f * std::abs(cv - cc));
		cluts[activeCLUT].saveTexture(renderer.getCLUTTexture());
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
		float center = static_cast<float>(x / renderer.getViewport().width);
		selected_ = cluts[activeCLUT].closestMarker(center);
	}

	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		float center = static_cast<float>(x / renderer.getViewport().width);
		selected_ = cluts[activeCLUT].closestMarker(center);

		auto cb = [&](const Color& color) {
			selected_->color(color);
			cluts[activeCLUT].saveTexture(renderer.getCLUTTexture());
			volumeRenderer->markDirty();
		};

		MainController::getInstance().pickColor(selected_->color(), cb);
	}
    
	return true;
}

bool Transfer1DController::leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame)
{
	finger_tracker_.update(leapController);

	if (finger_tracker_.tracking())
		return false;

	static auto lastSwipe = std::chrono::system_clock::now();
	Leap::GestureList gestures = currentFrame.gestures();
	for (Leap::Gesture g : gestures) {
		if (g.type() == Leap::Gesture::TYPE_SWIPE) {
			
			auto curTime = std::chrono::system_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - lastSwipe);
			Leap::SwipeGesture swipe(g);

			if (elapsed.count() > 1000 && g.isValid() && swipe.speed() > 2000 && std::abs(swipe.direction().z) < 0.5f) {
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

void Transfer1DController::moveAndScale(const Leap::Controller& controller)
{
	float c = saved_interval_.center() + finger_tracker_.centerPosDelta(controller.frame()).x / 400.0f;
	cluts[activeCLUT].interval().center(c);

	float w = std::max(0.0f, saved_interval_.width() + finger_tracker_.fingerGapDelta(controller.frame()) / 400.0f);
	cluts[activeCLUT].interval().width(w);

	cluts[activeCLUT].saveTexture(renderer.getCLUTTexture());
	volumeRenderer->markDirty();
}

void Transfer1DController::updateTransferTex1D()
{
    gl::Texture& tex = renderer.getTransferFn();
    tex.bind();
    tex.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    tex.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    tex.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    tex.setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
//    tex->setData2D(GL_RGB, texWidth, texHeight, GL_RGB, GL_UNSIGNED_BYTE, transfer1DPixels);
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
	renderer.setCLUT(&cluts[activeCLUT]);
	volumeRenderer->markDirty();
}

void Transfer1DController::prevCLUT()
{
	if (--activeCLUT < 0)
		activeCLUT = cluts.size() - 1;
	renderer.setCLUT(&cluts[activeCLUT]);
	volumeRenderer->markDirty();
}