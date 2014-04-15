#include "VolumeInfoRenderer.h"
#include "util/TextRenderer.h"
#include "main/MainController.h"
#include <sstream>
#include <iomanip>

using namespace std;

void VolumeInfoRenderer::init()
{
}

void VolumeInfoRenderer::setVolume(VolumeData* volume)
{
    this->volume = volume;
}

void VolumeInfoRenderer::setVolumeRenderer(VolumeRenderer* volumeRenderer)
{
    this->volumeRenderer = volumeRenderer;
}

void VolumeInfoRenderer::setSliceRenderer(SliceRenderer* sliceRenderer)
{
    this->sliceRenderer = sliceRenderer;
}

void VolumeInfoRenderer::resize(int width, int height)
{
    this->windowWidth = width;
    this->windowHeight = height;
}

void VolumeInfoRenderer::drawText(const std::string& s, int row)
{
    TextRenderer& textRenderer = MainController::getInstance().getText();
    
    int x = 0;
    int y = windowHeight - textRenderer.fontHeight() * row;
    textRenderer.add(s, x, y, TextRenderer::LEFT, TextRenderer::TOP);
}

void VolumeInfoRenderer::draw()
{
    TextRenderer& text = MainController::getInstance().getText();

	if (MainController::getInstance().getMode() == MainController::MODE_3D) {
		Vec3 c = Vec3(1.0f) - volumeRenderer->getBackgroundColor();
		text.setColor(c.x, c.y, c.z);
	}
	else {
		text.setColor(1, 1, 1);
	}

    
    text.begin(windowWidth, windowHeight);
    
	ostringstream os;

    int textRow = 0;
    
    // Patient Name (replace ^ characters with empty spaces)
    string name = volume->getValue<std::string, 0x0010, 0x0010>();
    replace(name.begin(), name.end(), '^', ' ');
	os << "Patient: " << name;
    drawText(os.str(), textRow++);
    
    // Modality
    switch (volume->getModality())
    {
        case VolumeData::CT:
            drawText(string("Modality: CT"), textRow++);
            break;
        case VolumeData::MR:
            drawText(string("Modality: MR"), textRow++);
            break;
        default:
            drawText(string("Modality: Unknown"), textRow++);
    }
    
    // Dimensions (voxels)
	os.str("");
	os << "Size (voxels): " << volume->getWidth() << " x " << volume->getHeight() << " x " << volume->getDepth();
    drawText(os.str(), textRow++);
    
    // Dimensions (mm)
    Vec3 v = volume->getDimensionsMM();
	os.str("");
	os << setprecision(1) << fixed;
	os << "Size (mm): " << v.x << " x " << v.y << " x " << v.z;
	drawText(os.str(), textRow++);

	// skip a row
    textRow++;
    
    // Rendering mode
    switch (volumeRenderer->getMode())
    {
        case VolumeRenderer::MIP:
            drawText(string("Rendering: MIP"), textRow++);
            break;
        case VolumeRenderer::VR:
            drawText(string("Rendering: VR"), textRow++);
            break;
        case VolumeRenderer::ISOSURFACE:
            drawText(string("Rendering: Isosurface"), textRow++);
            break;
        default:
            drawText(string("Rendering: Unknown"), textRow++);
    }
    
    // Rendering sample rate
	os.str("");
	os << "Samples: " << volumeRenderer->getCurrentNumSlices();
	drawText(os.str(), textRow++);

	// Stochastic Jitter
	os.str("");
	os << "Jitter: " << (volumeRenderer->useJitter ? "true" : "false");
	drawText(os.str(), textRow++);
    
    // VOI LUT (window) values
    float wc = volume->getCurrentWindow().getCenterReal();
    float ww = volume->getCurrentWindow().getWidthReal();
	os.str("");
	os << "WC,WW = " << wc << ", " << ww;
	drawText(os.str(), textRow++);
    
    // Slice Index (2D)
    if (MainController::getInstance().getMode() == MainController::MODE_2D) {
		os.str("");
		os << "Slice: " << (sliceRenderer->getCurrentSlice() + 1) << "/" << volume->getDepth();
		drawText(os.str(), textRow++);
    }
    
    text.end();
}