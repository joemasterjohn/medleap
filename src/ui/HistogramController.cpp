#include "HistogramController.h"

HistogramController::HistogramController() : histogram(NULL), transfer1DPixels(NULL)
{
    lMouseDrag = false;
    rMouseDrag = false;
}

HistogramController::~HistogramController()
{
    if (transfer1DPixels)
        delete transfer1DPixels;
}

HistogramRenderer* HistogramController::getRenderer()
{
    return &renderer;
}

void HistogramController::setVolume(VolumeData* volume)
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

bool HistogramController::mouseMotion(GLFWwindow* window, double x, double y)
{
    if (!renderer.getViewport().contains(x, y)) {
        renderer.setDrawCursor(false);
        return true;
    }
    
    renderer.setDrawCursor(true);
    
    renderer.setCursor(x, y);
    
    if (lMouseDrag) {
        double wc = (double)x / renderer.getViewport().width * histogram->getRange() + histogram->getMin();
        double ww = volume->getCurrentWindow().getWidthReal();
        volume->getCurrentWindow().setReal(wc, ww);
    } else if (rMouseDrag) {
        double cursorVal = (double)x / renderer.getViewport().width * histogram->getRange() + histogram->getMin();
        double wc = volume->getCurrentWindow().getCenterReal();
        double ww = 2 * std::abs(cursorVal - wc);
        volume->getCurrentWindow().setReal(wc, ww);
    }
    

    
    return true;
}

bool HistogramController::mouseButton(GLFWwindow* window, int button, int action, int mods)
{
    lMouseDrag = button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS;
    rMouseDrag = button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS;
    
    
    return true;
}

void HistogramController::updateTransferTex1D()
{
    cgl::Texture* tex = renderer.getTransferFn();
    tex->bind();
    tex->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    tex->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    tex->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    tex->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
//    tex->setData2D(GL_RGB, texWidth, texHeight, GL_RGB, GL_UNSIGNED_BYTE, transfer1DPixels);
}