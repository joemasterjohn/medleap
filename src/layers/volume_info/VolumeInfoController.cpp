#include "VolumeInfoController.h"

VolumeInfoController::VolumeInfoController()
{
}

VolumeInfoController::~VolumeInfoController()
{
}

VolumeInfoRenderer* VolumeInfoController::getRenderer()
{
    return &renderer;
}

void VolumeInfoController::setVolume(VolumeData* volume)
{
    renderer.setVolume(volume);
}