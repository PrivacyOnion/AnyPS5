#include "prx/libSceM4aacEnc/include/Api.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"
#include <array>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>

namespace {

void Require(bool value) { if (!value) std::abort(); }

template <typename Call>
bool Throws(Call call) {
    try {
        static_cast<void>(call());
    } catch (const std::runtime_error&) {
        return true;
    }
    return false;
}

}

int main() {
    uint32_t outputByte = 0x12345678;
    int32_t internalError = 0x5a5a5a5a;
    std::array<uint8_t, 64> inputBuffer{};
    std::array<uint8_t, 64> outputBuffer{};
    inputBuffer.fill(0xaa);
    outputBuffer.fill(0xbb);

    Require(Throws([&] { return sceM4aacEncClearContext(1); }));
    Require(Throws([&] { return sceM4aacEncClearContext(0); }));
    Require(Throws([&] { return sceM4aacEncClearContext(-1); }));
    Require(Throws([&] { return sceM4aacEncClearContextEx(); }));

    Require(Throws([&] { return sceM4aacEncCreateEncoder(2, 48000, 128000, 0, 0, &internalError); }));
    Require(Throws([&] { return sceM4aacEncCreateEncoder(2, 48000, 128000, 0, 0, nullptr); }));
    Require(Throws([&] { return sceM4aacEncCreateEncoder(1, 44100, 64000, 0, 0, &internalError); }));
    Require(Throws([&] { return sceM4aacEncCreateEncoder(6, 48000, 320000, 0, 0, &internalError); }));
    Require(Throws([&] { return sceM4aacEncCreateEncoder(0, 48000, 128000, 0, 0, &internalError); }));
    Require(Throws([&] { return sceM4aacEncCreateEncoder(2, 0, 128000, 0, 0, &internalError); }));
    Require(Throws([&] { return sceM4aacEncCreateEncoder(2, 48000, 0, 0, 0, &internalError); }));
    Require(Throws([&] { return sceM4aacEncCreateEncoderEx(); }));

    Require(Throws([&] { return sceM4aacEncDeleteEncoder(1, &internalError); }));
    Require(Throws([&] { return sceM4aacEncDeleteEncoder(1, nullptr); }));
    Require(Throws([&] { return sceM4aacEncDeleteEncoder(0, &internalError); }));
    Require(Throws([&] { return sceM4aacEncDeleteEncoder(-1, &internalError); }));
    Require(Throws([&] { return sceM4aacEncDeleteEncoderEx(); }));

    Require(Throws([&] { return sceM4aacEncEncode(1, nullptr, 0, nullptr, &outputByte, &internalError); }));
    Require(Throws([&] { return sceM4aacEncEncode(1, nullptr, 0, nullptr, nullptr, nullptr); }));
    Require(Throws([&] { return sceM4aacEncEncode(0, nullptr, 0, nullptr, &outputByte, &internalError); }));
    Require(Throws([&] { return sceM4aacEncEncode(-1, nullptr, 0, nullptr, &outputByte, &internalError); }));
    Require(Throws([&] { return sceM4aacEncEncode(1, inputBuffer.data(), static_cast<uint32_t>(inputBuffer.size()), outputBuffer.data(), &outputByte, &internalError); }));
    Require(Throws([&] { return sceM4aacEncEncodeEx(); }));
    Require(Throws([&] { return sceM4aacEncEncodeWithPriority(); }));
    Require(Throws([&] { return sceM4aacEncEncodeWithPriorityEx(); }));

    Require(Throws([&] { return sceM4aacEncFlush(1, nullptr, &outputByte, &internalError); }));
    Require(Throws([&] { return sceM4aacEncFlush(1, nullptr, nullptr, nullptr); }));
    Require(Throws([&] { return sceM4aacEncFlush(0, nullptr, &outputByte, &internalError); }));
    Require(Throws([&] { return sceM4aacEncFlush(-1, nullptr, &outputByte, &internalError); }));
    Require(Throws([&] { return sceM4aacEncFlush(1, outputBuffer.data(), &outputByte, &internalError); }));
    Require(Throws([&] { return sceM4aacEncFlushEx(); }));
    Require(Throws([&] { return sceM4aacEncFlushWithPriority(); }));
    Require(Throws([&] { return sceM4aacEncFlushWithPriorityEx(); }));

    Require(Throws([&] { return sceM4aacEncGetContext(); }));
    Require(Throws([&] { return sceM4aacEncInitialize(); }));
    Require(Throws([&] { return sceM4aacEncTerminate(); }));

    Require(outputByte == 0x12345678);
    Require(internalError == 0x5a5a5a5a);
    Require(inputBuffer[0] == 0xaa && inputBuffer.back() == 0xaa);
    Require(outputBuffer[0] == 0xbb && outputBuffer.back() == 0xbb);
}
