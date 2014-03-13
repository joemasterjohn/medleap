#include "VolumeInfoController.h"

VolumeInfoController::VolumeInfoController()
{
}

VolumeInfoController::~VolumeInfoController()
{
}

VolumeInfoRenderer* VolumeInfoController::getRenderLayer()
{
    return &renderer;
}

void VolumeInfoController::setVolume(VolumeData* volume)
{
    renderer.setVolume(volume);
}