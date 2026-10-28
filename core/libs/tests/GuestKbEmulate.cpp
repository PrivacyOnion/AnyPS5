#include "prx/libSceKbEmulate/include/Api.hpp"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string>

namespace {

void Require(bool condition, const char* message) {
    if (!condition) {
        std::fprintf(stderr, "KbEmulate: %s\n", message);
        std::abort();
    }
}

template <typename TCall>
void RequireNotImplemented(TCall call, const char* message) {
    try {
        call();
    } catch (const std::runtime_error&) {
        return;
    }
    std::fprintf(stderr, "KbEmulate: %s\n", message);
    std::abort();
}

}

int main() {
    const int32_t handle1 = sceKeyboardPadEmulateOpen_nid_postfix(0, nullptr);
    Require(handle1 > 0, "open returned non-positive handle");

    const int32_t handle2 = sceKeyboardPadEmulateOpen_nid_postfix(1, nullptr);
    Require(handle2 > 0, "open second handle returned non-positive handle");
    Require(handle1 != handle2, "handles must be unique");

    Require(sceKeyboardPadEmulateOpen_nid_postfix(-1, nullptr) < 0, "open with negative userId should fail");

    int dummy = 42;
    RequireNotImplemented([&] { sceKeyboardPadEmulateOpen_nid_postfix(0, &dummy); }, "open with non-null param must throw");

    Require(sceKeyboardPadEmulateClose_nid_postfix(handle1) == 0, "close handle1 failed");
    Require(sceKeyboardPadEmulateClose_nid_postfix(handle1) != 0, "closing already closed handle must fail");

    Require(sceKeyboardPadEmulateClose_nid_postfix(0) != 0, "closing handle 0 must fail");
    Require(sceKeyboardPadEmulateClose_nid_postfix(-1) != 0, "closing negative handle must fail");

    Require(sceKeyboardPadEmulateClose_nid_postfix(handle2) == 0, "close handle2 failed");
    Require(sceKeyboardPadEmulateClose_nid_postfix(handle2) != 0, "closing handle2 second time must fail");

    return 0;
}
