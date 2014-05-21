#include "VolumeInfoController.h"
#include "main/MainController.h"
#include <sstream>
#include <iomanip>

using namespace std;
using namespace gl;

VolumeInfoController::VolumeInfoController()
{
	text_.loadFont("menlo14");
}

void VolumeInfoController::setVolume(VolumeData* volume)
{
	this->volume = volume;
}

void VolumeInfoController::setVolumeRenderer(VolumeController* volumeRenderer)
{
	this->volumeRenderer = volumeRenderer;
}

void VolumeInfoController::setSliceRenderer(SliceController* sliceRenderer)
{
	this->sliceRenderer = sliceRenderer;
}

void VolumeInfoController::drawText(const std::string& s, int row)
{
	int x = 0;
	int y = viewport_.height - text_.fontHeight() * row;
	text_.add(s, x, y);
}

void VolumeInfoController::draw()
{
	if (!volume) {
		return;
	}

	Vec3 c = MainController::getInstance().getRenderer().getInverseBGColor();

	text_.clear();
	text_.viewport(viewport_);
	text_.color(c);
	text_.hAlign(TextRenderer::HAlign::left);
	text_.vAlign(TextRenderer::VAlign::top);

	ostringstream os;

	int textRow = 0;

	// Name
	os << "Data: " << volume->getName();
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
	// drawText("Size (mm): ", v.x, " x ", v.y, " x ", v.z);


	Vec3 v = volume->getSizeMillimeters();
	os.str("");
	os << setprecision(1) << fixed;
	os << "Size (mm): " << v.x << " x " << v.y << " x " << v.z;
	drawText(os.str(), textRow++);

	// skip a row
	textRow++;

	// Rendering mode
	switch (volumeRenderer->getMode())
	{
	case VolumeController::MIP:
		drawText(string("Rendering: MIP"), textRow++);
		break;
	case VolumeController::VR:
		drawText(string("Rendering: DVR"), textRow++);
		break;
	case VolumeController::ISOSURFACE:
		drawText(string("Rendering: Isosurface"), textRow++);
		break;
	default:
		drawText(string("Rendering: Unknown"), textRow++);
	}

	os.str("");
	if (volumeRenderer->getCamera().perspective()) {
		os << "Projection: Perspective" << endl;
	} else {
		os << "Projection: Orthographic" << endl;
	}
	drawText(os.str(), textRow++);

	// Rendering sample rate
	os.str("");
	os << "Samples: " << volumeRenderer->getCurrentNumSlices();
	drawText(os.str(), textRow++);

	// Stochastic Jitter
	os.str("");
	os << "Jitter: " << (volumeRenderer->useJitter ? "true" : "false");
	drawText(os.str(), textRow++);

	// VOI LUT (window) values
	int min = volume->getMinValue();
	int max = volume->getMaxValue();
	os.str("");
	os << "min, max = " << min << ", " << max;
	drawText(os.str(), textRow++);

	// Slice Index (2D)
	if (MainController::getInstance().getMode() == MainController::MODE_2D) {
		os.str("");
		os << "Slice: " << (sliceRenderer->slice() + 1) << "/" << volume->getDepth();
		drawText(os.str(), textRow++);
	}

	text_.draw();
}