#pragma once

#include "SceTypes.hpp"

extern "C" {

int APS5_VABI sceAudioInExtOpen_nid_postfix(int userId, uint32_t type, uint32_t index, uint32_t len, uint32_t freq, uint32_t param);
int32_t APS5_VABI sceAudioInExtClose_nid_postfix(int32_t handle);
int APS5_VABI sceAudioInExtInput_nid_postfix(int handle, void* dest);
int32_t APS5_VABI sceAudioInExtSetAecMode_nid_postfix(int32_t handle, int32_t mode);

}
