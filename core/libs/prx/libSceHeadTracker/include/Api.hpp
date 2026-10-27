#pragma once

#include "SceTypes.hpp"

extern "C" {

int APS5_VABI sceHeadTrackerFinalize();
int APS5_VABI sceHeadTrackerGetNumberOfRunningInstance();
int APS5_VABI sceHeadTrackerInitialize();
int APS5_VABI sceHeadTrackerQueryWorkingMemory();
int APS5_VABI sceHeadTrackerSearchCandidate();
int APS5_VABI sceHeadTrackerStart();
int APS5_VABI sceHeadTrackerStop();
int APS5_VABI sceHeadTrackerUpdate();

}
