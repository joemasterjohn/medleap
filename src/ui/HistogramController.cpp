#include "HistogramController.h"

HistogramController::HistogramController() : histogram(NULL)
{
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
    int numBins = 1024;
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
    
    histogram->print();
}