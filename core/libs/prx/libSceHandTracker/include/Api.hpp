#pragma once

#include "SceTypes.hpp"

extern "C" {

int APS5_VABI sceHandTrackerGetDataMemorySize();
int APS5_VABI sceHandTrackerGetDefaultParam();
int APS5_VABI sceHandTrackerGetDepthRoi();
int APS5_VABI sceHandTrackerInitialize();
int APS5_VABI sceHandTrackerSetDepthInfo();
int APS5_VABI sceHandTrackerSetFaceInfo();

}
