#include "prx/libSceMsgDialog/include/Api.hpp"
#include "SceTypes.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace {

void Require(bool value) { if (!value) std::abort(); }

constexpr int kStatusRunning = 2;
constexpr int kStatusFinished = 3;
constexpr int kErrOk = 0;
constexpr int kErrNotRunning = static_cast<int>(0x80B8000Bu);
constexpr int kErrArgNull = static_cast<int>(0x80B8000Du);
constexpr int kModeUserMsg = 1;
constexpr int kButtonIdOk = 1;
constexpr int kButtonIdNo = 2;
constexpr int kButtonTypeYesNoFocusNo = 4;

}

int main() {
    MsgDialogResult result{};
    std::uint8_t param[0x60]{};
    std::uint8_t userMsg[16]{};
    const char text[] = "Test message";

    Require(sceMsgDialogGetResult_nid_postfix(nullptr) == kErrArgNull);
    Require(sceMsgDialogGetResult_nid_postfix(&result) == kErrNotRunning);

    Require(sceMsgDialogInitialize() == kErrOk);
    Require(sceMsgDialogGetResult_nid_postfix(nullptr) == kErrArgNull);
    Require(sceMsgDialogGetResult_nid_postfix(&result) == kErrNotRunning);

    Require(sceMsgDialogOpen(nullptr) == kErrArgNull);
    Require(sceMsgDialogOpen(param) == kErrOk);
    Require(sceMsgDialogGetResult_nid_postfix(nullptr) == kErrArgNull);
    Require(sceMsgDialogGetResult_nid_postfix(&result) == kErrNotRunning);

    Require(sceMsgDialogUpdateStatus() == kStatusRunning);
    Require(sceMsgDialogGetResult_nid_postfix(&result) == kErrNotRunning);

    Require(sceMsgDialogUpdateStatus() == kStatusFinished);
    Require(sceMsgDialogGetResult_nid_postfix(nullptr) == kErrArgNull);

    result = MsgDialogResult{};
    result.mode = -1;
    result.result = -1;
    result.button_id = -1;
    Require(sceMsgDialogGetResult_nid_postfix(&result) == kErrOk);
    Require(result.mode == kModeUserMsg);
    Require(result.result == kErrOk);
    Require(result.button_id == kButtonIdOk);

    std::int32_t buttonType = kButtonTypeYesNoFocusNo;
    std::memcpy(userMsg, &buttonType, sizeof(buttonType));
    const char* pText = text;
    std::memcpy(userMsg + 8, &pText, sizeof(pText));
    std::int32_t mode = kModeUserMsg;
    std::memcpy(param + 0x38, &mode, sizeof(mode));
    const std::uint8_t* pUserMsg = userMsg;
    std::memcpy(param + 0x40, &pUserMsg, sizeof(pUserMsg));

    Require(sceMsgDialogOpen(param) == kErrOk);
    Require(sceMsgDialogGetResult_nid_postfix(&result) == kErrNotRunning);
    Require(sceMsgDialogClose() == kErrOk);
    Require(sceMsgDialogGetStatus() == kStatusFinished);

    result = MsgDialogResult{};
    result.mode = -1;
    result.result = -1;
    result.button_id = -1;
    Require(sceMsgDialogGetResult_nid_postfix(&result) == kErrOk);
    Require(result.mode == kModeUserMsg);
    Require(result.result == kErrOk);
    Require(result.button_id == kButtonIdNo);

    Require(sceMsgDialogTerminate() == kErrOk);
    Require(sceMsgDialogGetResult_nid_postfix(nullptr) == kErrArgNull);
    Require(sceMsgDialogGetResult_nid_postfix(&result) == kErrNotRunning);
    return 0;
}
