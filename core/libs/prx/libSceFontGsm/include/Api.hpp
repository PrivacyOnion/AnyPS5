#pragma once

#include "SceTypes.hpp"

extern "C" {

int APS5_VABI sceFontGraphicsAgcDrawupFillTextureImage();
int APS5_VABI sceFontGraphicsAgcDrawupFillTexturePattern();
int APS5_VABI sceFontGraphicsAgcSurfaceInit();
int APS5_VABI sceFontGraphicsSequenceGetAgcCommandsForJump();
int APS5_VABI sceFontGraphicsSequenceGetAgcCommandsForSubmit();
int APS5_VABI sceFontSelectGraphicsAgc();

}
