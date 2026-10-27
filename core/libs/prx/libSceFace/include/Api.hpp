#pragma once

#include "SceTypes.hpp"

extern "C" {

int APS5_VABI sceFaceAgeGetWorkingMemorySize();
int APS5_VABI sceFaceAgeRangeIntegrate();
int APS5_VABI sceFaceAllPartsGetWorkingMemorySize();
int APS5_VABI sceFaceAttributeGetWorkingMemorySize();
int APS5_VABI sceFaceDetectionGetDefaultParam();
int APS5_VABI sceFaceDetectionGetWorkingMemorySize();
int APS5_VABI sceFaceIdentifyExGetWorkingMemorySize();
int APS5_VABI sceFaceIdentifyGetWorkingMemorySize();
int APS5_VABI sceFaceIdentifyLiteGetWorkingMemorySize();
int APS5_VABI sceFacePartsGetWorkingMemorySize();
int APS5_VABI sceFaceShapeGetWorkingMemorySize();
int APS5_VABI sceFaceShapeTrack();

}
