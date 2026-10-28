#include "prx/libSceSaveDataDialog/include/Api.hpp"
#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

namespace {
constexpr int kStatusNone = 0;
constexpr int kStatusInitialized = 1;
constexpr int kStatusRunning = 2;
constexpr int kStatusFinished = 3;

int g_status = kStatusNone;
}

extern "C" {

int APS5_VABI sceSaveDataDialogGetStatus(void);
int APS5_VABI sceSaveDataDialogUpdateStatus(void);

int APS5_VABI sceSaveDataDialogGetStatus_nid_postfix(void) {
 g_status = sceSaveDataDialogGetStatus();
 return g_status;
}

int APS5_VABI sceSaveDataDialogUpdateStatus_nid_postfix(void) {
 g_status = sceSaveDataDialogUpdateStatus();
 return g_status;
}

}
