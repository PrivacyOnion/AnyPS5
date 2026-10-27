#include "prx/libSceAt9Enc/include/Api.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <string>

namespace {

void Require(bool value) { if (!value) std::abort(); }

template <typename Call>
bool ReportsNotImplemented(const char* name, Call call) {
    try {
        static_cast<void>(call());
    } catch (const std::runtime_error& error) {
        return std::string(error.what()) == std::string(name) + " not implemented";
    }
    return false;
}

}

int main() {
    int32_t memSize = 0;
    int32_t internalResult = 0;

    Require(ReportsNotImplemented("sceAt9EncClearContext", [&] { return sceAt9EncClearContext(nullptr, &internalResult); }));
    Require(ReportsNotImplemented("sceAt9EncClearContext", [&] { return sceAt9EncClearContext(nullptr, nullptr); }));

    Require(ReportsNotImplemented("sceAt9EncCreateEncoder", [&] { return sceAt9EncCreateEncoder(nullptr, nullptr, 0, &internalResult); }));
    Require(ReportsNotImplemented("sceAt9EncCreateEncoder", [&] { return sceAt9EncCreateEncoder(nullptr, nullptr, 0, nullptr); }));
    Require(ReportsNotImplemented("sceAt9EncCreateEncoder", [&] { return sceAt9EncCreateEncoder(nullptr, nullptr, -1, nullptr); }));

    Require(ReportsNotImplemented("sceAt9EncEncode", [&] { return sceAt9EncEncode(nullptr, nullptr, nullptr, 0, 0, nullptr, nullptr, &internalResult); }));
    Require(ReportsNotImplemented("sceAt9EncEncode", [&] { return sceAt9EncEncode(nullptr, nullptr, nullptr, 0, 0, nullptr, nullptr, nullptr); }));

    Require(ReportsNotImplemented("sceAt9EncFlush", [&] { return sceAt9EncFlush(nullptr, nullptr, 0, nullptr, &internalResult); }));
    Require(ReportsNotImplemented("sceAt9EncFlush", [&] { return sceAt9EncFlush(nullptr, nullptr, 0, nullptr, nullptr); }));

    Require(ReportsNotImplemented("sceAt9EncQueryMemSize", [&] { return sceAt9EncQueryMemSize(nullptr, &memSize, &internalResult); }));
    Require(ReportsNotImplemented("sceAt9EncQueryMemSize", [&] { return sceAt9EncQueryMemSize(nullptr, nullptr, nullptr); }));
    Require(ReportsNotImplemented("sceAt9EncQueryMemSize", [&] { return sceAt9EncQueryMemSize(nullptr, &memSize, nullptr); }));

    Require(memSize == 0);
    Require(internalResult == 0);

    uint8_t workArea[128] = {0xAA};
    uint8_t param[64] = {0xBB};
    uint8_t inputBuf[128] = {0xCC};
    uint8_t outputBuf[128] = {0xDD};
    uint32_t outputDataByte = 0x12345678;
    uint32_t usedBufSize = 0x87654321;
    int32_t sentinelMemSize = 0x55AA55AA;
    int32_t sentinelInternalResult = 0x778899AA;

    Require(ReportsNotImplemented("sceAt9EncClearContext", [&] { return sceAt9EncClearContext(workArea, &sentinelInternalResult); }));
    Require(ReportsNotImplemented("sceAt9EncCreateEncoder", [&] { return sceAt9EncCreateEncoder(workArea, param, static_cast<int32_t>(sizeof(workArea)), &sentinelInternalResult); }));
    Require(ReportsNotImplemented("sceAt9EncCreateEncoder", [&] { return sceAt9EncCreateEncoder(workArea, param, -1, &sentinelInternalResult); }));
    Require(ReportsNotImplemented("sceAt9EncEncode", [&] { return sceAt9EncEncode(workArea, inputBuf, outputBuf, static_cast<uint32_t>(sizeof(inputBuf)), static_cast<uint32_t>(sizeof(outputBuf)), &outputDataByte, &usedBufSize, &sentinelInternalResult); }));
    Require(ReportsNotImplemented("sceAt9EncEncode", [&] { return sceAt9EncEncode(workArea, nullptr, nullptr, 256, 256, &outputDataByte, &usedBufSize, &sentinelInternalResult); }));
    Require(ReportsNotImplemented("sceAt9EncFlush", [&] { return sceAt9EncFlush(workArea, outputBuf, static_cast<uint32_t>(sizeof(outputBuf)), &outputDataByte, &sentinelInternalResult); }));
    Require(ReportsNotImplemented("sceAt9EncFlush", [&] { return sceAt9EncFlush(workArea, nullptr, 256, &outputDataByte, &sentinelInternalResult); }));
    Require(ReportsNotImplemented("sceAt9EncQueryMemSize", [&] { return sceAt9EncQueryMemSize(param, &sentinelMemSize, &sentinelInternalResult); }));

    Require(workArea[0] == 0xAA);
    Require(param[0] == 0xBB);
    Require(inputBuf[0] == 0xCC);
    Require(outputBuf[0] == 0xDD);
    Require(outputDataByte == 0x12345678);
    Require(usedBufSize == 0x87654321);
    Require(sentinelMemSize == static_cast<int32_t>(0x55AA55AA));
    Require(sentinelInternalResult == static_cast<int32_t>(0x778899AA));
}
