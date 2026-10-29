#pragma once

#include "SceTypes.hpp"
#include <cstddef>

struct Asn1String;
struct LibreSslAsn1Object;

struct OcspResponseBytes {
    LibreSslAsn1Object* responseType;
    Asn1String* response;
};

struct OcspResponse {
    Asn1String* responseStatus;
    OcspResponseBytes* responseBytes;
};

static_assert(sizeof(OcspResponse) == 16);
static_assert(offsetof(OcspResponse, responseBytes) == 8);
static_assert(sizeof(OcspResponseBytes) == 16);

extern "C" void APS5_VABI OCSP_RESPONSE_free_nid_postfix(OcspResponse* response);
