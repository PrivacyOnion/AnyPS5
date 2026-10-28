#pragma once

#include "SceTypes.hpp"
#include <cstdint>

extern "C" {

int APS5_VABI sceKeyboardPadEmulateOpen_nid_postfix(int32_t userId, const void* param);
int APS5_VABI sceKeyboardPadEmulateClose_nid_postfix(int32_t handle);

}
