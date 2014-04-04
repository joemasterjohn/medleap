#include "VolumeInfoRenderer.h"
#include "util/TextRenderer.h"
#include "main/MainController.h"

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

void VolumeInfoRenderer::drawText(const char* text, int row)
{
    TextRenderer& textRenderer = MainController::getInstance().getText();
    
    int x = 0;
    int y = windowHeight - textRenderer.measureHeight(text) * row;
    textRenderer.add(text, x, y, TextRenderer::LEFT, TextRenderer::TOP);
}

void VolumeInfoRenderer::draw()
{
    TextRenderer& text = MainController::getInstance().getText();
    text.setColor(1, 1, 1);
    text.begin(windowWidth, windowHeight);
    
    char textBuf[128];
    int textRow = 0;
    
    // Patient Name
    std::string name = volume->getValue<std::string, 0x0010, 0x0010>();
    std::replace(name.begin(), name.end(), '^', ' ');
    sprintf(textBuf, "Patient: %s", name.c_str());
    drawText(textBuf, textRow++);
    
    // Modality
    switch (volume->getModality())
    {
        case VolumeData::CT:
            drawText("Modality: CT", textRow++);
            break;
        case VolumeData::MR:
            drawText("Modality: MR", textRow++);
            break;
        default:
            drawText("Modality: Unknown", textRow++);
    }
    
    // Dimensions (voxels)
    sprintf(textBuf, "Size (voxels): %d x %d x %d", volume->getWidth(), volume->getHeight(), volume->getDepth());
    drawText(textBuf, textRow++);
    
    // Dimensions (mm)
    Vec3 v = volume->getDimensionsMM();
    sprintf(textBuf, "Size (mm): %.1f x %.1f x %.1f", v.x, v.y, v.z);
    drawText(textBuf, textRow++);
    
    textRow++;
    
    // Rendering mode
    switch (volumeRenderer->getMode())
    {
        case VolumeRenderer::MIP:
            drawText("Rendering: MIP", textRow++);
            break;
        case VolumeRenderer::VR:
            drawText("Rendering: VR", textRow++);
            break;
        default:
            drawText("Rendering: Unknown", textRow++);
    }
    
    // Rendering sample rate
    sprintf(textBuf, "Samples: %d", volumeRenderer->getNumSamples());
    drawText(textBuf, textRow++);
    
    // VOI LUT (window) values
    float wc = volume->getCurrentWindow().getCenterReal();
    float ww = volume->getCurrentWindow().getWidthReal();
    sprintf(textBuf, "WC = %.1f  WW = %.1f", wc, ww);
    drawText(textBuf, textRow++);
    
    // Slice Index (2D)
    sprintf(textBuf, "Slice: %d/%d", sliceRenderer->getCurrentSlice()+1, volume->getDepth());
    drawText(textBuf, textRow++);
    
    text.end();
}