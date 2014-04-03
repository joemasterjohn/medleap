#include "VolumeInfoRenderer.h"
#include "util/TextRenderer.h"
#include "ui/MainController.h"

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

void VolumeInfoRenderer::resize(int width, int height)
{
    this->windowWidth = width;
    this->windowHeight = height;
}

void VolumeInfoRenderer::draw()
{
    TextRenderer& text = MainController::getInstance().getText();
    
    text.setColor(1, 1, 1);
    text.begin(windowWidth, windowHeight);
    
    char buf[60];
    
    // VOI LUT (window) values
    float wc = volume->getCurrentWindow().getCenterReal();
    float ww = volume->getCurrentWindow().getWidthReal();
    sprintf(buf, "WC = %.1f  WW = %.1f", wc, ww);
    text.add(buf, windowWidth, 0, TextRenderer::RIGHT, TextRenderer::BOTTOM);
    
    // Patient Name
    std::string name = volume->getValue<std::string, 0x0010, 0x0010>();
    std::replace(name.begin(), name.end(), '^', ' ');
    text.add(("Patient: " + name).c_str(), 0, windowHeight, TextRenderer::LEFT, TextRenderer::TOP);
    
    // Modality
    switch (volume->getModality())
    {
        case VolumeData::CT:
            text.add("Modality: CT", 0, windowHeight - 18, TextRenderer::LEFT, TextRenderer::TOP);
            break;
        case VolumeData::MR:
            text.add("Modality: MR", 0, windowHeight - 18, TextRenderer::LEFT, TextRenderer::TOP);
            break;
        default:
            text.add("Modality: UNKNOWN", 0, windowHeight - 18, TextRenderer::LEFT, TextRenderer::TOP);
    }
    
    // Rendering mode
    if (volumeRenderer->getMode() == VolumeRenderer::MIP)
        text.add("Render Mode: MIP", 0, windowHeight - 18 * 2, TextRenderer::LEFT, TextRenderer::TOP);
    else
        text.add("Render Mode: VR", 0, windowHeight - 18 * 2, TextRenderer::LEFT, TextRenderer::TOP);
    
    // Rendering sample rate
    sprintf(buf, "Sampling Planes: %d", volumeRenderer->getNumSamples());
    text.add(buf, 0, windowHeight - 18 * 3, TextRenderer::LEFT, TextRenderer::TOP);
    
    
    // Dimensions (voxels)
    double numVoxels = volume->getNumVoxels() / 1000.0;
    char unit = 'K';
    if (volume->getNumVoxels() > 1000.0) {
        numVoxels /= 1000.0;
        unit = 'M';
    }
    sprintf(buf, "Volume Dimensions (voxels): %d x %d x %d (%.1f%c total)",
            volume->getWidth(),
            volume->getHeight(),
            volume->getDepth(),
            numVoxels,
            unit);
    text.add(buf, windowWidth, windowHeight, TextRenderer::RIGHT, TextRenderer::TOP);
    
    // Dimensions (mm)
    Vec3 v = volume->getDimensionsMM();
    sprintf(buf, "Volume Dimensions (mm): %.1f x %.1f x %.1f", v.x, v.y, v.z);
    text.add(buf, windowWidth, windowHeight - 18, TextRenderer::RIGHT, TextRenderer::TOP);
    
    // Voxel Size
    const Vec3 vsize = volume->getVoxelSize();
    sprintf(buf, "Voxel Size (mm): %.2f x %.2f x %.2f", vsize.x, vsize.y, vsize.z);
    text.add(buf, windowWidth, windowHeight - 36, TextRenderer::RIGHT, TextRenderer::TOP);
    
    text.end();
}