#pragma once

#include "SceTypes.hpp"

extern "C" {

int APS5_VABI sceFaceTrackerAddUserFeature();
int APS5_VABI sceFaceTrackerFinalize();
int APS5_VABI sceFaceTrackerGetDefaultConfig();
int APS5_VABI sceFaceTrackerGetResult();
int APS5_VABI sceFaceTrackerGetTrackIdByUserId();
int APS5_VABI sceFaceTrackerGetUserFeature();
int APS5_VABI sceFaceTrackerGetUserIdByTrackId();
int APS5_VABI sceFaceTrackerGetWorkingMemorySize();
int APS5_VABI sceFaceTrackerInitialize();
int APS5_VABI sceFaceTrackerRegisterFixUserIdCallback();
int APS5_VABI sceFaceTrackerRegisterLostTargetCallback();
int APS5_VABI sceFaceTrackerRegisterRecoveryFromLostTargetCallback();
int APS5_VABI sceFaceTrackerRegisterStartTrackingCallback();
int APS5_VABI sceFaceTrackerRegisterStopTrackingCallback();
int APS5_VABI sceFaceTrackerRegisterUser();
int APS5_VABI sceFaceTrackerReset();
int APS5_VABI sceFaceTrackerSearchTarget();
int APS5_VABI sceFaceTrackerStartTracking();
int APS5_VABI sceFaceTrackerStopTracking();
int APS5_VABI sceFaceTrackerUnregisterUser();
int APS5_VABI sceFaceTrackerUpdate();

}
