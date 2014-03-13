#include "VolumeInfoRenderer.h"

void VolumeInfoRenderer::init()
{
    text.loadFont("menlo14");
}

void VolumeInfoRenderer::setVolume(VolumeData* volume)
{
    this->volume = volume;
}

void VolumeInfoRenderer::resize(int width, int height)
{
    this->windowWidth = width;
    this->windowHeight = height;
}

void VolumeInfoRenderer::draw()
{
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
    text.add(name.c_str(), 0, windowHeight, TextRenderer::LEFT, TextRenderer::TOP);
    
    // Voxel Size
    const cgl::Vec3 vsize = volume->getVoxelSize();
    sprintf(buf, "Voxel Size: %.2f, %.2f, %.2f", vsize.x, vsize.y, vsize.z);
    text.add(buf, windowWidth, windowHeight, TextRenderer::RIGHT, TextRenderer::TOP);
    
    // Image Size
    sprintf(buf, "Image Size: %d x %d", volume->getWidth(), volume->getHeight());
    text.add(buf, windowWidth, windowHeight - 18, TextRenderer::RIGHT, TextRenderer::TOP);
    
    text.end();
}