#include "prx/libSceLibreSsl/include/Api.hpp"
#include <cstdint>
#include <stdexcept>

namespace {

constexpr int DynamicObject = 1;
constexpr int DynamicStrings = 4;
constexpr int DynamicData = 8;
constexpr int BorrowedStringData = 16;

void Wipe(void* pointer, std::int32_t length) {
    auto* bytes = static_cast<volatile unsigned char*>(pointer);
    for (std::int32_t index = 0; index < length; ++index) bytes[index] = 0;
}

void ValidateString(const Asn1String* string) {
    if (string != nullptr && string->data != nullptr && !(string->flags & BorrowedStringData) && string->length < 0)
        throw std::invalid_argument("OCSP_RESPONSE_free: negative owned string length");
}

void Validate(const OcspResponse& response) {
    ValidateString(response.responseStatus);
    if (response.responseBytes == nullptr) return;
    ValidateString(response.responseBytes->response);
    const auto* object = response.responseBytes->responseType;
    if (object != nullptr && (object->flags & DynamicData) && object->data != nullptr && object->length < 0)
        throw std::invalid_argument("OCSP_RESPONSE_free: negative owned object length");
}

void FreeString(Asn1String* string) {
    if (string == nullptr) return;
    if (string->data != nullptr && !(string->flags & BorrowedStringData)) {
        Wipe(string->data, string->length);
        CRYPTO_free_nid_postfix(string->data);
    }
    CRYPTO_free_nid_postfix(string);
}

void FreeObject(LibreSslAsn1Object* object) {
    if (object == nullptr) return;
    if (object->flags & DynamicStrings) {
        CRYPTO_free_nid_postfix(const_cast<char*>(object->shortName));
        CRYPTO_free_nid_postfix(const_cast<char*>(object->longName));
        object->shortName = nullptr;
        object->longName = nullptr;
    }
    if (object->flags & DynamicData) {
        if (object->data != nullptr) {
            auto* data = const_cast<unsigned char*>(object->data);
            Wipe(data, object->length);
            CRYPTO_free_nid_postfix(data);
        }
        object->data = nullptr;
        object->length = 0;
    }
    if (object->flags & DynamicObject) CRYPTO_free_nid_postfix(object);
}

} // namespace

extern "C" void APS5_VABI OCSP_RESPONSE_free_nid_postfix(OcspResponse* response) {
    if (response == nullptr) return;
    Validate(*response);
    if (auto* bytes = response->responseBytes) {
        FreeString(bytes->response);
        FreeObject(bytes->responseType);
        CRYPTO_free_nid_postfix(bytes);
    }
    FreeString(response->responseStatus);
    CRYPTO_free_nid_postfix(response);
}
