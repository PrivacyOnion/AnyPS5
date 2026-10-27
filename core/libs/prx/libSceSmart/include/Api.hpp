#pragma once

#include "SceTypes.hpp"

extern "C" {

int APS5_VABI sceSmartCreateInstantImageTarget();
int APS5_VABI sceSmartCreateLearnedImageTarget();
int APS5_VABI sceSmartDestroyTarget();
int APS5_VABI sceSmartGetTargetInfo();
int APS5_VABI sceSmartInit();
int APS5_VABI sceSmartRelease();
int APS5_VABI sceSmartTargetSelectorCreateTargetPack();
int APS5_VABI sceSmartTargetSelectorDestroyTargetPack();
int APS5_VABI sceSmartTargetSelectorGetResults();
int APS5_VABI sceSmartTargetSelectorGetTargetPackInfo();
int APS5_VABI sceSmartTargetSelectorQuery();
int APS5_VABI sceSmartTargetSelectorRegisterTargetPack();
int APS5_VABI sceSmartTargetSelectorReset();
int APS5_VABI sceSmartTargetSelectorRun();
int APS5_VABI sceSmartTargetSelectorStart();
int APS5_VABI sceSmartTargetSelectorStop();
int APS5_VABI sceSmartTargetSelectorUnregisterTargetPack();
int APS5_VABI sceSmartTargetTrackingCreateInstance();
int APS5_VABI sceSmartTargetTrackingDestroyInstance();
int APS5_VABI sceSmartTargetTrackingDispatchAndQuery();
int APS5_VABI sceSmartTargetTrackingDispatchAndQueryMI();
int APS5_VABI sceSmartTargetTrackingGetDefaultInstanceId();
int APS5_VABI sceSmartTargetTrackingGetResults();
int APS5_VABI sceSmartTargetTrackingGetResultsMI();
int APS5_VABI sceSmartTargetTrackingQuery();
int APS5_VABI sceSmartTargetTrackingQuery2();
int APS5_VABI sceSmartTargetTrackingQuery2MI();
int APS5_VABI sceSmartTargetTrackingQueryMI();
int APS5_VABI sceSmartTargetTrackingRegisterTarget();
int APS5_VABI sceSmartTargetTrackingRegisterTargetMI();
int APS5_VABI sceSmartTargetTrackingReset();
int APS5_VABI sceSmartTargetTrackingResetMI();
int APS5_VABI sceSmartTargetTrackingRun();
int APS5_VABI sceSmartTargetTrackingRun2();
int APS5_VABI sceSmartTargetTrackingRun2MI();
int APS5_VABI sceSmartTargetTrackingRunMI();
int APS5_VABI sceSmartTargetTrackingRunWorker();
int APS5_VABI sceSmartTargetTrackingRunWorkerMI();
int APS5_VABI sceSmartTargetTrackingSetCameraParam();
int APS5_VABI sceSmartTargetTrackingSetCameraParamMI();
int APS5_VABI sceSmartTargetTrackingSetSearchPolicy();
int APS5_VABI sceSmartTargetTrackingSetSearchPolicyMI();
int APS5_VABI sceSmartTargetTrackingStart();
int APS5_VABI sceSmartTargetTrackingStartMI();
int APS5_VABI sceSmartTargetTrackingStop();
int APS5_VABI sceSmartTargetTrackingStopMI();
int APS5_VABI sceSmartTargetTrackingUnregisterTarget();
int APS5_VABI sceSmartTargetTrackingUnregisterTargetMI();

}
