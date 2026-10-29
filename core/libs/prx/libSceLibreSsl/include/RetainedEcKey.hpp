#pragma once

#include "SceTypes.hpp"
#include <cstddef>
#include <cstdint>

struct EcKey;

struct LibreSslEvpPkey {
    std::int32_t type;
    std::int32_t savedType;
    std::int32_t references;
    const void* method;
    void* engine;
    void* key;
    std::int32_t saveParameters;
    void* attributes;
};

static_assert(sizeof(LibreSslEvpPkey) == 56);
static_assert(offsetof(LibreSslEvpPkey, key) == 32);

extern "C" EcKey* APS5_VABI EVP_PKEY_get1_EC_KEY_nid_postfix(LibreSslEvpPkey* key);
