#include "prx/libSceKbEmulate/include/Api.hpp"
#include <cstdint>
#include <mutex>
#include <unordered_set>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

namespace {

std::mutex emulateMutex;
std::unordered_set<int32_t> openHandles;
int32_t nextHandle = 1;

}

extern "C" {

int APS5_VABI sceKeyboardPadEmulateOpen_nid_postfix(int32_t userId, const void* param) {
    if (param != nullptr) {
        NotImplemented_nid_no_patch(__func__);
    }
    if (userId < 0) {
        return -1;
    }
    std::lock_guard lock(emulateMutex);
    const int32_t handle = nextHandle++;
    openHandles.insert(handle);
    return handle;
}

int APS5_VABI sceKeyboardPadEmulateClose_nid_postfix(int32_t handle) {
    if (handle <= 0) {
        return -1;
    }
    std::lock_guard lock(emulateMutex);
    const auto it = openHandles.find(handle);
    if (it == openHandles.end()) {
        return -1;
    }
    openHandles.erase(it);
    return 0;
}

}
