#include "prx/libSceAudiodec.native/include/Api.hpp"
#include <cstdint>
#include <cstddef>
#include <mutex>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

namespace {

constexpr std::int32_t ERROR_ALREADY_INITIALIZED = static_cast<std::int32_t>(0x807F0003);

std::mutex g_mutex;
bool g_initialized = false;

} // namespace

extern "C" {

int APS5_VABI sceAudiodecClearContextEx(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAudiodecCreateDecoderEx(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAudiodecDecode2(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAudiodecDecode2Ex(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAudiodecDecode2WithPriority(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAudiodecDecode2WithPriorityEx(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAudiodecDecodeEx(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAudiodecDecodeWithPriority(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAudiodecDecodeWithPriorityEx(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAudiodecDeleteDecoderEx(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAudiodecGetContext(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAudiodecInitLibrary_nid_postfix(void* param) {
    (void)param;
    std::lock_guard lock(g_mutex);
    if (g_initialized) {
        return ERROR_ALREADY_INITIALIZED;
    }
    g_initialized = true;
    return 0;
}

int APS5_VABI sceAudiodecInitialize(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAudiodecRegisterCodec(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAudiodecTermLibrary_nid_postfix(void) {
    std::lock_guard lock(g_mutex);
    g_initialized = false;
    return 0;
}

int APS5_VABI sceAudiodecTerminate(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

}
