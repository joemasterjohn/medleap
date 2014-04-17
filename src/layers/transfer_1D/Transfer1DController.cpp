#include "Transfer1DController.h"

Transfer1DController::Transfer1DController() : histogram(NULL), transfer1DPixels(NULL)
{
    lMouseDrag = false;
    rMouseDrag = false;
    
    
    volumeRenderer = NULL;
    
    // start out with the default CLUT: black->white gradient
    cluts.push_back(CLUT());
    
    CLUT redToWhite;
    redToWhite.addColorStop(0.25f, Vec4(1, 0, 0, 1));
    cluts.push_back(redToWhite);
    
    CLUT test;
    test.addColorStop(0.2f, Vec4(0, 1, 0, .8f));
    test.addColorStop(0.4f, Vec4(1, 1, 0, .9f));
    test.addColorStop(0.6f, Vec4(1, 0, 0, 1));
    cluts.push_back(test);

    CLUT spectrum;
    spectrum.clearStops();
    spectrum.addColorStop(0,              Vec4(164, 37, 138, 1)/255.0f);
    spectrum.addColorStop(0.1428571429*1, Vec4(0, 92, 166, 50)/255.0f);
    spectrum.addColorStop(0.1428571429*2, Vec4(42, 199, 239, 50)/255.0f);
    spectrum.addColorStop(0.1428571429*3, Vec4(0, 168, 96, 170)/255.0f);
    spectrum.addColorStop(0.1428571429*4, Vec4(251, 240, 82, 22)/255.0f);
    spectrum.addColorStop(0.1428571429*5, Vec4(254, 135, 59, 180)/255.0f);
    spectrum.addColorStop(0.1428571429*6, Vec4(247, 18, 42, 128)/255.0f);
    spectrum.addColorStop(1.0f,           Vec4(169, 0, 36, 5)/255.0f);
    cluts.push_back(spectrum);


	CLUT ryw;
	ryw.clearStops();
	ryw.addColorStop(0.0f, Vec4(0, 0, 0, 0.0f));
	ryw.addColorStop(0.2f, Vec4(0.8f, 0, 0, 0.7f));
	ryw.addColorStop(0.5f, Vec4(1, 1, 0, 0.8f));
	ryw.addColorStop(1.0f, Vec4(1, 1, 1, 1.0f));
	cluts.push_back(ryw);
    
    CLUT lowTail;
	lowTail.clearStops();
	lowTail.addColorStop(0.0f, Vec4(0, 0, 0, 0.0f));
	lowTail.addColorStop(0.2f, Vec4(1, 0, 0, 0.1f));
	lowTail.addColorStop(0.5f, Vec4(1, 1, 0, 0.8f));
	lowTail.addColorStop(1.0f, Vec4(1, 1, 1, 1.0f));
	cluts.push_back(lowTail);
    
	CLUT foo;
	foo.clearStops();
	foo.addColorStop(0.0f, Vec4(0, 0, 0, 0.0f));
	foo.addColorStop(0.2f, Vec4(0, 0, 0.8f, 0.2f));
	foo.addColorStop(0.8f, Vec4(1, 1, 0, 0.8f));
	foo.addColorStop(1.0f, Vec4(1, 1, 1, 0.9f));
	cluts.push_back(foo);


    CLUT mid;
    mid.clearStops();
    mid.addColorStop(0.0f, Vec4(0, 0, 0, 0));
    mid.addColorStop(0.5f, Vec4(1, 1, 1, 1));
    mid.addColorStop(1.0f, Vec4(0, 0, 0, 0));
    cluts.push_back(mid);
    
    CLUT yellowToWhite;
    yellowToWhite.clearStops();
    yellowToWhite.addColorStop(0.0f, Vec4(1, 0, 0, 1));
    yellowToWhite.addColorStop(1.0f, Vec4(0, 1, 0, 0.1f));
    cluts.push_back(yellowToWhite);
    
    CLUT monochrome;
    monochrome.clearStops();
    monochrome.addColorStop(0.0f, Vec4(1, 1, 1, 0));
    monochrome.addColorStop(1.0f, Vec4(1, 1, 1, 0.2f));
    cluts.push_back(monochrome);
    
    renderer.setCLUT(&cluts[activeCLUT = 0]);
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

void Transfer1DController::setVolume(VolumeData* volume)
{
    this->volume = volume;
    
    if (histogram)
        delete histogram;
    
    // 1024 bins is somewhat arbitrary; consider adding customization later
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
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        activeCLUT = (activeCLUT + 1) % cluts.size();
        renderer.setCLUT(&cluts[activeCLUT]);
        volumeRenderer->markDirty();
    }
    
    return true;
}

bool Transfer1DController::mouseMotion(GLFWwindow* window, double x, double y)
{
    if (!renderer.getViewport().contains(x, y)) {
        renderer.setDrawCursor(false);
        return true;
    }
    
    renderer.setDrawCursor(true);
    
    renderer.setCursor(static_cast<int>(x), static_cast<int>(y));
    
    if (lMouseDrag) {
		float wc = static_cast<float>(x) / renderer.getViewport().width * histogram->getRange() + histogram->getMin();
		float ww = volume->getCurrentWindow().getWidthReal();
        volume->getCurrentWindow().setReal(wc, ww);
        volumeRenderer->markDirty();
    } else if (rMouseDrag) {
		float cursorVal = static_cast<float>(x) / renderer.getViewport().width * histogram->getRange() + histogram->getMin();
		float wc = volume->getCurrentWindow().getCenterReal();
		float ww = 2 * std::abs(cursorVal - wc);
        volume->getCurrentWindow().setReal(wc, ww);
        volumeRenderer->markDirty();
    }
    

    
    return true;
}

bool Transfer1DController::mouseButton(GLFWwindow* window, int button, int action, int mods)
{
    lMouseDrag = button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS;
    rMouseDrag = button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS;
    
    
	return true;
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